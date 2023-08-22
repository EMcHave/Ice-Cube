#include "pch.h"
#include "LogicClass.h"


#define pidiv4 Directx::XM_PIDIV4;

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
	m_gravity = XMFLOAT3(0, -0.1, 0);

	E = 2 * pow(10, 4);
	G = 2 * pow(10, 2);
	nu = 0.3;
	float rho = 900;
	m = 1;
	a = 0.5;
	J = 1;
	Jp = 5 * pow(10, -10);
	S = 0.0025;
	k = pow(10, 8);




	int NX = 10;
	int NY = 20;
	int NZ = 1;

	InitialConditions initCond1 { XMFLOAT3(0, 0, 4), XMFLOAT3(0.25, 0, 0) };
	InitialConditions initCond2 { XMFLOAT3(4.5, -0.5, 1.5), XMFLOAT3(0, 0, 0) };
	//InitialConditions initCond3 { XMFLOAT3(6, 0, 12), XMFLOAT3(0, 0, -0.2) };
	Material mat{ E, G, nu, rho };
	
	auto entity1 = std::make_shared<Entity>(XMFLOAT4(NX, NY, NZ, 0.5 * a), initCond1, mat, EntityType::PlaneTriangle);
	//auto entity2 = std::make_shared<Entity>(XMFLOAT4(NX, NY, NZ, a), initCond3, mat, EntityType::PlaneTriangle);
	auto entity3 = std::make_shared<Entity>(XMFLOAT4(2, 6, 10, 0.25 * a), initCond2, mat, EntityType::Cylinder);
	//auto entity3 = std::make_shared<Entity>(XMFLOAT4(1.5, 11, 10, a), initCond2, mat, EntityType::Cylinder);

	m_entities.push_back(entity1);
	//m_entities.push_back(entity2);
	m_entities.push_back(entity3);

	XMFLOAT3 meshSize(50, 50, 10);
	m_mesh = Ice::Mesh(meshSize, 2 * max(m_entities[0]->Size().w, m_entities[1]->Size().w));


	for (auto entity : m_entities)
	{
		for (int i = 0; i < entity->Particles().size(); i++)
			m_particles.push_back(entity->Particles()[i]);
		/*
		for (int i = 0; i < entity->Connections().size(); i++)
			m_connections.push_back(entity->Connections()[i]);
			*/
	}


	
	m_isFirstTimeStep = true;
}

void LogicClass::TimeStep()
{
	using namespace DirectX;
	/*
	for (auto e : m_entities)
		for (auto p : e->Particles())
		{
			DirectX::XMFLOAT3 pos = p->Position();
			std::shared_ptr<Ice::Cell> cell = m_mesh.CellByPosition(p->Position());
			if (cell.get() != nullptr)
				cell->AddParticle(p.get());
		}
	*/

	for (int e = 0; e < m_entities.size(); e++)
	{
		auto entity = m_entities[e];

		for (auto it = entity->Connections().begin(); it!=entity->Connections().end(); )
		{
			auto connection = *it;
			float dist = GeometricObject::DistanceIJ(connection->p1().get(), connection->p2().get());
			
			if (entity->CheckBreak(dist, connection))
			{
				connection->Break(entity->ContactParticles());
				it = entity->Connections().erase(it);
				continue;
			}
			
			XMVECTOR r_ij = connection->p2()->VectorPosition() - connection->p1()->VectorPosition();
			XMVECTOR e_ij = r_ij / dist;
			XMVECTOR f_ij = entity->B1() * (dist - entity->Size().w) * e_ij +
				entity->B2() / 2 / dist * (connection->n_i2(1) - connection->n_i1(1) - XMVector3Dot(e_ij, connection->n_i2(1) - connection->n_i1(1)) * e_ij);
			XMVECTOR MTB = entity->B3() * XMVector3Cross(connection->n_i2(1), connection->n_i1(1)) -
				entity->B4() * 0.5 * (XMVector3Cross(connection->n_i2(2), connection->n_i1(2)) + XMVector3Cross(connection->n_i2(3), connection->n_i1(3)));
			XMVECTOR m_ij = -entity->B2() * 0.5 * XMVector3Cross(e_ij, connection->n_i1(1)) + MTB;
			XMVECTOR m_ji = entity->B2() * 0.5 * XMVector3Cross(e_ij, connection->n_i2(1)) - MTB;

			connection->p1()->Force() += f_ij;
			connection->p2()->Force() -= f_ij;
			connection->p1()->Moment() += m_ij;
			connection->p2()->Moment() += m_ji;

			++it;
			
			//connection->p1()->Moment() += XMVector3Transform(m_ij, XMMatrixTranspose(XMMatrixRotationQuaternion(connection->p1()->VectorQuaternion())));
			//connection->p2()->Moment() += XMVector3Transform(m_ji, XMMatrixTranspose(XMMatrixRotationQuaternion(connection->p2()->VectorQuaternion())));
			
		}	
	
		for (int i = 0; i < (int)entity->Size().x; i++)
			for (int j = 0; j < (int)entity->Size().y; j++)
				if (entity->Particle(i, j, 0)->Position().y < 0)
				{
					auto arch_force = XMFLOAT3(0,
						-5 * m_gravity.y * abs(entity->Particle(i, j, 0)->Position().y * (entity->Size().x - 1) * (entity->Size().y - 1) * entity->Size().w),
						0);
					entity->Particle(i, j, 0)->Force() += XMLoadFloat3(&arch_force);
				}
		

		
		
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
		
		for (int i = 0; i < entity->ContactParticles().size(); i++)
			for (int j = i + 1; j < entity->ContactParticles().size(); j++)
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
		
		/*
		for (auto cell : m_mesh.Cells())
			for (int i = 0; i < cell->Particles().size(); i++)
				for (int j = i + 1; j < cell->Particles().size(); j++)
				{
					Cube* p1 = dynamic_cast<Cube*>(cell->Particles().at(i));
					Cube* p2 = dynamic_cast<Cube*>(cell->Particles().at(j));
					if (!Entity::CheckIfConnectionExists(p1, p2))
					{
						float r = GeometricObject::DistanceIJ(p1, p2);
						float f = Entity::LennardJones(r, p1->R() + p2->R(), 0.1);
						DirectX::XMVECTOR e_ij = (p2->VectorPosition() - p1->VectorPosition()) / r;
						p1->Force() += f * e_ij;
						p2->Force() -= f * e_ij;
					}
				}
		*/

		


		
		for (auto particle : entity->Particles())
		{
			particle->Force() += XMLoadFloat3(&m_gravity);

			if (particle->Position().y > 0)
				vis = 0;
			else
				vis = 0.0005;

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
		
		for (auto connection : entity->Connections())
			connection->Update();
		for (auto particle : entity->Particles())
			particle->ResetForcesAndMoments();
			
		if (m_isFirstTimeStep)
			m_isFirstTimeStep = false;
	}
	/*
	for (auto cell : m_mesh.Cells())
		if (!cell->isEmpty)
			cell->Clear();
			*/
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

