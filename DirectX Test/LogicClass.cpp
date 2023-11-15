#include "pch.h"
#include "LogicClass.h"
#include <execution>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace DirectX;
LogicClass::LogicClass()
{
	
}

void LogicClass::Init(
	std::shared_ptr<MoveLookController> const& controller,
	std::shared_ptr<MyRenderer> const& renderer)
{
	m_controller = controller;
	m_renderer = renderer;
	m_camera.SetProjParams(70.0f * 3.1415 / 180.0f, 1.0f, 0.01f, 100.0f);
	m_gravity = XMFLOAT3(0, -1, 0);
	m_threads.reserve(8);
	m_asyncThreads.reserve(8);


	E = 2 * pow(10, 4);
	G = 2 * pow(10, 2);
	nu = 0.3;
	float rho = 900;
	float M = 1000;
	m = 5;
	
	J = 1;
	Jp = 5 * pow(10, -10);
	S = 0.0025;
	k = pow(10, 8);

	float iceLength = 4;
	float iceWidth = 2;
	float iceHeight;

	float cylinderRadius = 1;
	float cylinderNumberOfParticles = 100;
	float cylinderHeight = 0.7;

	int NX = 60;
	int NY = 30;
	int NZ = 1;

	float a1 = iceLength / NX;
	float a2 = 2 * XM_PI * cylinderRadius / cylinderNumberOfParticles;

	InitialConditions initCond1 { XMFLOAT3(0.1, 0, 2), XMFLOAT3(0.01, 0, 0) };
	InitialConditions initCond2 { XMFLOAT3(1.05, -0.5, 0), XMFLOAT3(0, 0, 0) };
	InitialConditions initCond3 { XMFLOAT3(0.175, -0.1, 2.5), XMFLOAT3(0, 0, 0) };
	Material mat{ E, G, nu, rho };
	
	XMFLOAT3 ny(0, 1, 0);
	XMFLOAT3 nz(0, 0, 1);
	XMVECTOR quat1 = XMQuaternionRotationNormal(XMLoadFloat3(&ny), XM_PIDIV2);
	XMVECTOR quat2 = XMQuaternionRotationNormal(XMLoadFloat3(&nz), XM_PI / 2);
	XMFLOAT4 f_totalQuat1;
	XMFLOAT4 f_totalQuat2;
	//XMStoreFloat4(&f_totalQuat, XMQuaternionMultiply(quat2, quat1));
	XMStoreFloat4(&f_totalQuat1, quat1);
	XMStoreFloat4(&f_totalQuat2, quat2);
	//f_totalQuat = XMFLOAT4(0, 0, 0, 1);


	auto entity1 = std::make_shared<Block>(
		XMFLOAT4(0.53f, 0.81f, 0.94f, 0.0f),
		initCond1,
		mat,
		Behavior::Flexible,
		XMFLOAT4(iceLength, iceWidth, a1, a1),
		f_totalQuat1
	);
	
	auto entity2 = std::make_shared<Cylinder>(
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		initCond2,
		mat,
		Behavior::Rigid,
		XMFLOAT4(cylinderRadius, 0, cylinderHeight, a2),
		60
	);

	auto entity3 = std::make_shared<Block>(
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		initCond3,
		mat,
		Behavior::Rigid,
		XMFLOAT4(1, 5, a1, a1),
		f_totalQuat2
	);

	m_entities.push_back(entity1);
	//m_entities.push_back(entity2);
	m_entities.push_back(entity3);

	const XMFLOAT3 meshSize(5, 2, 5);
	const float cellSize = max(m_entities[0]->Size().w, m_entities[1]->Size().w);
	//const float cellSize = m_entities[0]->Size().w;
	m_mesh = Ice::Mesh(meshSize, 2 * cellSize);
	
	for (auto entity : m_entities)
		for (int i = 0; i < entity->Particles().size(); i++)
			m_particles.push_back(entity->Particles()[i]);

	m_isFirstTimeStep = true;
}

void LogicClass::TimeStep()
{
	using namespace DirectX;

	for (auto& particle : m_particles)
		particle->ResetForcesAndMoments();

	
	if (m_interactionModel == InteractionModel::MeshBased)
	{
		for(auto& p : m_particles)
		{
			DirectX::XMFLOAT3 pos = p->Position();
			std::shared_ptr<Ice::Cell> cell = m_mesh.CellByPosition(p->Position());
			if (cell.get() != nullptr)
				cell->AddParticle(p.get());
		}
		
		//ALLCELLS(i, j, k)
			//EvaluateMeshBasedInteraction(i, j, k);

		ParallelMeshBasedInteraction();
	}



	for (auto& entity : m_entities)
	{
		//auto entity = m_entities[e];
		bool is_flexible = entity->GetBehavior() == Behavior::Flexible;

		for (auto it = entity->Connections().begin(); it != entity->Connections().end(); )
		{
			auto connection = *it;
			float dist = GeometricObject::DistanceIJ(connection->p1().get(), connection->p2().get());

			if (connection->CheckBreak(dist) && entity->GetBehavior() != Behavior::Rigid)
			{
				connection->Break(entity->ContactParticles());
				it = entity->Connections().erase(it);
				continue;
			}

			XMVECTOR r_ij = connection->p2()->VectorPosition() - connection->p1()->VectorPosition();
			XMVECTOR e_ij = r_ij / dist;
			XMVECTOR f_ij = entity->B1() * (1 + connection->Delta()) * (dist - entity->Size().w) * e_ij +
				entity->B2() * (1 + connection->Delta()) * 0.5 / dist * (connection->n_i2(1) - connection->n_i1(1) - XMVector3Dot(e_ij, connection->n_i2(1) - connection->n_i1(1)) * e_ij);
			XMVECTOR MTB = entity->B3() * (1 + connection->Delta()) * XMVector3Cross(connection->n_i2(1), connection->n_i1(1)) -
				entity->B4() * (1 + connection->Delta()) * 0.5 * (XMVector3Cross(connection->n_i2(2), connection->n_i1(2)) + XMVector3Cross(connection->n_i2(3), connection->n_i1(3)));
			XMVECTOR m_ij = -entity->B2() * (1 + connection->Delta()) * 0.5 * XMVector3Cross(e_ij, connection->n_i1(1)) + MTB;
			XMVECTOR m_ji = entity->B2() * (1 + connection->Delta()) * 0.5 * XMVector3Cross(e_ij, connection->n_i2(1)) - MTB;

			connection->p1()->Force() += f_ij * is_flexible;
			connection->p2()->Force() -= f_ij * is_flexible;
			connection->p1()->Moment() += m_ij * is_flexible;
			connection->p2()->Moment() += m_ji * is_flexible;

			++it;

			//connection->p1()->Moment() += XMVector3Transform(m_ij, XMMatrixTranspose(XMMatrixRotationQuaternion(connection->p1()->VectorQuaternion())));
			//connection->p2()->Moment() += XMVector3Transform(m_ji, XMMatrixTranspose(XMMatrixRotationQuaternion(connection->p2()->VectorQuaternion())));

		}

		if (entity->GetBehavior() != Behavior::Rigid)
			EvaluateArcForce(entity);

		for (auto& particle : entity->Particles())
		{
			if (entity->GetBehavior() != Behavior::Rigid)
				particle->Force() += XMLoadFloat3(&m_gravity);

			if (particle->Position().y > 0)
				vis = 0;
			else
				vis = 0;

			XMFLOAT3 constraint(1, 1, 1);
			if (m_isFirstTimeStep)
			{
				particle->Velocity(particle->VectorVelocity() + 0.5 * dt * particle->Force() / m);
				//particle->AngularVelocity(particle->VectorVelocity() + 0.5 * dt * particle->Moment() / J);
			}

			TranslationalIntegratorLeapFrog(particle);
			RotationalIntegratorSymplectic(particle);

			if (!m_realTimeRender)
				particle->AddTimePoint(particle->VectorPosition(), particle->VectorQuaternion());
		}
		


		//for (auto& connection : entity->Connections())
			//connection->Update();
		std::for_each(std::execution::par_unseq,
			entity->Connections().begin(),
			entity->Connections().end(),
			[](std::shared_ptr<Connection>& c) {c->Update(); });

		if (m_isFirstTimeStep)
			m_isFirstTimeStep = false;
	}

	if (m_interactionModel == InteractionModel::MeshBased)
		for (auto& cell : m_mesh.Cells())
			if (!cell->isEmpty)
				cell->Clear();			
}

void LogicClass::WriteForceToFile(std::ofstream& file)
{
	XMVECTOR integralForce = XMVectorZero();
	for (auto p : m_entities[1]->Particles())
		integralForce += p->Force();
	XMFLOAT3 f_integralForce;
	XMStoreFloat3(&f_integralForce, integralForce);
	file << f_integralForce.x << '\t'
		 << f_integralForce.y << '\t'
		 << f_integralForce.z << '\n';
}
/*
void LogicClass::WriteDiagramToFile(std::ofstream& file)
{
	std::shared_ptr<Entity> e = m_entities[1];

	if (auto* c = dynamic_cast<Cylinder*>(e.get()))
		for (int j = 0; j < c->Size().y; j++)
		{
			XMVECTOR integralForce = XMVectorZero();

			for (int k = 0; k < c->Size().z; k++)
				integralForce += c->Particle(0, j, k)->Force();
			XMFLOAT3 f_integralForce;
			XMStoreFloat3(&f_integralForce, integralForce);
			file << j << ','
				 << c->Particle(0, j, 0)->Position().x << ','
				 << c->Particle(0, j, 0)->Position().y << ','
				 << c->Particle(0, j, 0)->Position().z << ','
				 << f_integralForce.x << ','
				 << f_integralForce.y << ','
				 << f_integralForce.z << '\n';
		}
	else if (Block* c = dynamic_cast<Block*>(e.get()))
	{
		for (int j = 0; j < c->Size().y; j++)
		{
			XMVECTOR integralForce = XMVectorZero();
			for (int i = 0; i < c->Size().x; i++)
				integralForce += c->Particle(i, j, 0)->Force();
			XMFLOAT3 f_integralForce;
			XMStoreFloat3(&f_integralForce, integralForce);
			file << j << ','
				<< c->Particle(0, j, 0)->Position().x << ','
				<< c->Particle(0, j, 0)->Position().y << ','
				<< c->Particle(0, j, 0)->Position().z << ','
				<< f_integralForce.x << ','
				<< f_integralForce.y << ','
				<< f_integralForce.z << '\n';
		}
	}
}
*/
void LogicClass::WriteDiagramToFile(std::ofstream& file)
{
	std::shared_ptr<Entity> e = m_entities[1];

	if (auto* c = dynamic_cast<Cylinder*>(e.get()))
		for (int j = 0; j < c->Size().y; j++)
		{
			XMVECTOR integralForce;

			for (int k = 0; k < c->Size().z; k++)
			{
				integralForce = c->Particle(0, j, k)->Force();
				XMFLOAT3 f_integralForce;
				XMStoreFloat3(&f_integralForce, integralForce);
				file << j << ','
					<< c->Particle(0, j, k)->Position().x << ','
					<< c->Particle(0, j, k)->Position().y << ','
					<< c->Particle(0, j, k)->Position().z << ','
					<< f_integralForce.x << ','
					<< f_integralForce.y << ','
					<< f_integralForce.z << '\n';
			}
		}
	else if (Block* c = dynamic_cast<Block*>(e.get()))
	{
		for (int j = 0; j < c->Size().y; j++)
		{
			XMVECTOR integralForce;
			for (int i = 0; i < c->Size().x; i++)
			{
				integralForce = c->Particle(i, j, 0)->Force();
				XMFLOAT3 f_integralForce;
				XMStoreFloat3(&f_integralForce, integralForce);
				file << j << ','
					<< c->Particle(i, j, 0)->Position().x << ','
					<< c->Particle(i, j, 0)->Position().y << ','
					<< c->Particle(i, j, 0)->Position().z << ','
					<< f_integralForce.x << ','
					<< f_integralForce.y << ','
					<< f_integralForce.z << '\n';
			}
		}
	}
}

void LogicClass::AnimationStep(int i)
{
	for (auto p : m_particles)
	{
		p->Position(p->m_positions[i]);
		p->Quaternion(p->m_orientations[i]);
	}
}

void LogicClass::RotationalIntegratorSymplectic(const std::shared_ptr<Cube> particle)
{

	XMVECTOR av_prev = particle->VectorAngularVelocity() * !m_isFirstTimeStep + (!particle->m_isFixed) * particle->Moment() / J * (0.5 * dt);
	XMStoreFloat3(&m_tempAngularVelocity3, av_prev);
	m_tempAngularVelocity4 = XMFLOAT4(m_tempAngularVelocity3.x, m_tempAngularVelocity3.y, m_tempAngularVelocity3.z, 0.f);
	XMVECTOR q_half_plus = particle->VectorQuaternion() + (0.5 * dt) * 0.5 * XMQuaternionMultiply(particle->VectorQuaternion(), XMLoadFloat4(&m_tempAngularVelocity4));

	XMVECTOR av_half_plus = particle->VectorAngularVelocity() * (!particle->m_isFixed) * !m_isFirstTimeStep + dt * particle->Moment() / J;
	XMStoreFloat3(&m_tempAngularVelocity3, av_half_plus);
	m_tempAngularVelocity4 = XMFLOAT4(m_tempAngularVelocity3.x, m_tempAngularVelocity3.y, m_tempAngularVelocity3.z, 0.f);
	XMVECTOR q_new = particle->VectorQuaternion() + 0.5 * dt * XMQuaternionMultiply(q_half_plus, XMLoadFloat4(&m_tempAngularVelocity4));

	particle->AngularVelocity(av_half_plus);
	particle->Quaternion(q_new / XMQuaternionLength(q_new));

	/*
	XMVECTOR L_prev_global = particle->KineticMoment() + (0.5 * dt) * particle->Moment();
	XMVECTOR L_prev_local = XMVector3Transform(L_prev_global, XMMatrixRotationQuaternion(particle->VectorQuaternion()));
	XMStoreFloat3(&m_tempAngularVelocity3, L_prev_local/J);
	m_tempAngularVelocity4 = XMFLOAT4(m_tempAngularVelocity3.x, m_tempAngularVelocity3.y, m_tempAngularVelocity3.z, 0.f);
	XMVECTOR q_half_plus = particle->VectorQuaternion() + (0.5 * dt) * 0.5 * XMQuaternionMultiply(particle->VectorQuaternion(), XMLoadFloat4(&m_tempAngularVelocity4));

	XMVECTOR L_half_plus_global = particle->KineticMoment() + dt * particle->Moment();
	XMVECTOR L_half_plus_local = XMVector3Transform(L_half_plus_global, XMMatrixRotationQuaternion(q_half_plus));
	XMStoreFloat3(&m_tempAngularVelocity3, L_half_plus_local / J);
	m_tempAngularVelocity4 = XMFLOAT4(m_tempAngularVelocity3.x, m_tempAngularVelocity3.y, m_tempAngularVelocity3.z, 0.f);
	XMVECTOR q_new = particle->VectorQuaternion() + 0.5 * dt * XMQuaternionMultiply(q_half_plus, XMLoadFloat4(&m_tempAngularVelocity4));

	particle->KineticMoment(L_half_plus_global);
	particle->Quaternion(q_new / XMQuaternionLength(q_new));
	*/
}

void LogicClass::RotationalIntegratorNonSymplectic(const std::shared_ptr<Cube> particle)
{
	XMVECTOR av_new = particle->VectorAngularVelocity() + particle->Moment() / J * dt;
	XMStoreFloat3(&m_tempAngularVelocity3, av_new);
	m_tempAngularVelocity4 = XMFLOAT4(m_tempAngularVelocity3.x, m_tempAngularVelocity3.y, m_tempAngularVelocity3.z, 0.f);
	XMVECTOR q_new = particle->VectorQuaternion() + 0.5 * XMQuaternionMultiply(XMLoadFloat4(&m_tempAngularVelocity4), particle->VectorQuaternion()) * dt;

	particle->AngularVelocity(av_new);
	particle->Quaternion(q_new / XMQuaternionLength(q_new));
}

void LogicClass::TranslationalIntegratorLeapFrog(const std::shared_ptr<Cube> particle)
{
	XMVECTOR v_new = particle->VectorVelocity() + (-vis * particle->VectorVelocity() + (!particle->m_isFixed) * particle->Force() / m) * dt * !m_isFirstTimeStep;
	XMVECTOR r_new = particle->VectorPosition() + v_new * dt;

	particle->Velocity(v_new);
	particle->Position(r_new);
}

void LogicClass::EvaluateInteractionBetweenEntities(int e)
{
	auto entity = m_entities[e];
	for (int ae = e + 1; ae < m_entities.size(); ae++)
	{
		auto another_entity = m_entities[ae];
		for (auto particle : entity->ContactParticles())
			for (auto another_particle : another_entity->ContactParticles())
			{
				float r = GeometricObject::DistanceIJ(particle.get(), another_particle.get());
				float f = Entity::LennardJones(r, particle->R() + another_particle->R(), 0.1);
				DirectX::XMVECTOR e_ij = (another_particle->VectorPosition() - particle->VectorPosition()) / r;
				particle->Force() += f * e_ij;
				another_particle->Force() -= f * e_ij;
			}
	}
}

void LogicClass::EvaluateInteractionInEntity(
	const std::shared_ptr<Entity> entity)
{
	
	if (entity->GetBehavior() != Behavior::Rigid)
	{
		int size = entity->ContactParticles().size();
		for (int i = 0; i < size; i++)
			for (int j = i + 1; j < size; j++)
			{
				auto p1 = entity->ContactParticles().at(i);
				auto p2 = entity->ContactParticles().at(j);

				if (!Entity::CheckIfConnectionExists(p1.get(), p2.get()))
				{
					float r = GeometricObject::DistanceIJ(p1.get(), p2.get());
					float f = Entity::LennardJones(r, p1->R() + p2->R(), 0.1);
					DirectX::XMVECTOR e_ij = (p2->VectorPosition() - p1->VectorPosition()) / r;
					p1->Force() += f * e_ij;
					p2->Force() -= f * e_ij;
				}

			}
	}
}

void LogicClass::EvaluateMeshBasedInteraction(int i, int j, int k)
{	
	for (auto& particle : m_mesh.Cel(i, j, k)->Particles())
	{
		Cube* p1 = dynamic_cast<Cube*>(particle);
		CELLSAROUND(i, j, k, e1, e2, e3)
			for (auto& another_particle : m_mesh.Cel(e1, e2, e3)->Particles())
			{
				Cube* p2 = dynamic_cast<Cube*>(another_particle);

				if (p1 != p2 && !Entity::CheckIfConnectionExists(p1, p2))
				{
					float r = GeometricObject::DistanceIJ(p1, p2);
					float f = Entity::LennardJones(r, p1->R() + p2->R(), 0.1);
					DirectX::XMVECTOR e_ij = (p2->VectorPosition() - p1->VectorPosition()) / r;
					p1->Force() += f * e_ij;
				}
				
			}
	}
}

void LogicClass::ParallelMeshBasedInteraction()
{	
	using namespace std;
	
	m_asyncThreads.clear();
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, 1.f, 1.f, 1.f, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z / 2));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, 1.f, 1.f, m_mesh.Size().x - 1, m_mesh.Size().y / 2, m_mesh.Size().z / 2));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, 1.f, m_mesh.Size().y / 2, 1.f, m_mesh.Size().x / 2, m_mesh.Size().y - 1, m_mesh.Size().z / 2));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, m_mesh.Size().y / 2, 1.f, m_mesh.Size().x - 1, m_mesh.Size().y - 1, m_mesh.Size().z / 2));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, 1.f, 1.f, m_mesh.Size().z / 2, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z - 1));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, 1.f, m_mesh.Size().z / 2, m_mesh.Size().x - 1, m_mesh.Size().y / 2, m_mesh.Size().z - 1));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, 1.f, m_mesh.Size().y / 2, m_mesh.Size().z / 2, m_mesh.Size().x / 2, m_mesh.Size().y - 1, m_mesh.Size().z - 1));
	m_asyncThreads.push_back(async(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z / 2, m_mesh.Size().x - 1, m_mesh.Size().y - 1, m_mesh.Size().z - 1));
	
	for (auto& future : m_asyncThreads)
		future.wait();
	/*
	m_threads.emplace_back(&LogicClass::CreateThread, this, 1.f, 1.f, 1.f, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z / 2);
	m_threads.emplace_back(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, 1.f, 1.f, m_mesh.Size().x - 1, m_mesh.Size().y / 2, m_mesh.Size().z / 2);
	m_threads.emplace_back(&LogicClass::CreateThread, this, 1.f, m_mesh.Size().y / 2, 1.f, m_mesh.Size().x / 2, m_mesh.Size().y - 1, m_mesh.Size().z / 2);
	m_threads.emplace_back(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, m_mesh.Size().y / 2, 1.f, m_mesh.Size().x - 1, m_mesh.Size().y - 1, m_mesh.Size().z / 2);
	m_threads.emplace_back(&LogicClass::CreateThread, this, 1.f, 1.f, m_mesh.Size().z / 2, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z - 1);
	m_threads.emplace_back(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, 1.f, m_mesh.Size().z / 2, m_mesh.Size().x - 1, m_mesh.Size().y / 2, m_mesh.Size().z - 1);
	m_threads.emplace_back(&LogicClass::CreateThread, this, 1.f, m_mesh.Size().y / 2, m_mesh.Size().z / 2, m_mesh.Size().x / 2, m_mesh.Size().y - 1, m_mesh.Size().z - 1);
	m_threads.emplace_back(&LogicClass::CreateThread, this, m_mesh.Size().x / 2, m_mesh.Size().y / 2, m_mesh.Size().z / 2, m_mesh.Size().x - 1, m_mesh.Size().y - 1, m_mesh.Size().z - 1);
	
	for (auto& th : m_threads)
		if (th.joinable())
			th.join();
	*/		

}

void LogicClass::EvaluateArcForce(const std::shared_ptr<Entity> entity)
{
	float V = 0;
	for (auto p : entity->Particles())
	{
		if (p->Position().y <= -p->R())
			V += 8 * p->R() * p->R() * p->R();
		else if (p->Position().y > p->R())
			V += 0;
		else
			V += 4 * p->R() * p->R() * (p->R() - p->Position().y);
	}

	for (int i = 0; i < (int)entity->Size().x; i++)
		for (int j = 0; j < (int)entity->Size().y; j++)
			if (entity->Particle(i, j, 0)->Position().y < 0)
			{
				int number_of_bottom_particles = (int)entity->Size().x * (int)entity->Size().y;
				auto arch_force = XMFLOAT3(0,
					-10 * m_gravity.y * V,
					0);
				entity->Particle(i, j, 0)->Force() += XMLoadFloat3(&arch_force);
			}
}

void LogicClass::CreateThread(float bl, float bw, float bh, float el, float ew, float eh)
{
	ALLCELLSUPD(i, j, k, bl, bw, bh, el, ew, eh)
		EvaluateMeshBasedInteraction(i, j, k);
}

