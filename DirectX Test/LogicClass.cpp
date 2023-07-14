#include "pch.h"
#include "LogicClass.h"

#define pidiv4 Directx::XM_PIDIV4;

using namespace DirectX;
LogicClass::LogicClass()
{

}

void LogicClass::Init(std::shared_ptr<MoveLookController> const& controller, std::shared_ptr<MyRenderer> const& renderer)
{
	m_controller = controller;
	m_renderer = renderer;
	m_camera.SetProjParams(70.0f * 3.1415 / 180.0f, 1.0f, 0.01f, 100.0f);
	

	E = 20;
	G = 20;
	nu = 0.3;
	m = 1;
	a = 1;
	J = 1;
	Jp = 5 * pow(10, -10);
	S = 0.0025;
	k = pow(10, 8);
	dt = pow(10, -3);

	int NX = 3;
	int NY = 3;
	int NZ = 3;

	vis = 0;

	//XMVECTOR axis = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&n3), 0);
	
	auto entity1 = std::make_shared<Entity>(XMFLOAT4(NX, NY, NZ, a), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0));
	auto entity2 = std::make_shared<Entity>(XMFLOAT4(NX, NY, NZ, a), XMFLOAT3(6, 0, 6), XMFLOAT3(0, 0, -1));
	auto entity3 = std::make_shared<Entity>(XMFLOAT4(NX, NY, NZ, a), XMFLOAT3(6, 0, 0), XMFLOAT3(0, 0, 0));
	

	m_entities.push_back(entity1);
	m_entities.push_back(entity2);
	//m_entities.push_back(entity3);
	for (auto entity : m_entities)
	{
		for (int i = 0; i < entity->Particles().size(); i++)
			m_particles.push_back(entity->Particles()[i]);
		for (int i = 0; i < entity->Connections().size(); i++)
			m_connections.push_back(entity->Connections()[i]);
	}
	//m_objects[8]->Position(XMFLOAT3(4, 1, -4));
	//m_particles[m_particles.size()-1]->Position(XMFLOAT3((NX - 1) * a - 0.1, (NY - 1) * a + 0.1, - (NZ - 1) * a));
	//for (auto obj : m_objects)
		//obj->Velocity(XMFLOAT3(0.1, 0, 0));
	//m_particles[0]->AngularVelocity(XMFLOAT3(0.5, 0, 0));
}

void LogicClass::TimeStep()
{
	using namespace DirectX;
	for (auto entity : m_entities)
	{
		for (auto connection : entity->Connections())
		{
			float dist = GeometricObject::DistanceIJ(connection->p1(), connection->p2());
			XMVECTOR r_ij = connection->p2()->VectorPosition() - connection->p1()->VectorPosition();
			XMVECTOR e_ij = r_ij / dist;
			XMVECTOR f_ij = B1() * (dist - a) * e_ij +
				B2() / 2 / dist * (connection->n_i2(1) - connection->n_i1(1) - XMVector3Dot(e_ij, connection->n_i2(1) - connection->n_i1(1)) * e_ij);
			XMVECTOR MTB = B3() * XMVector3Cross(connection->n_i2(1), connection->n_i1(1)) -
				B4() * 0.5 * (XMVector3Cross(connection->n_i2(2), connection->n_i1(2)) + XMVector3Cross(connection->n_i2(3), connection->n_i1(3)));
			XMVECTOR m_ij = -B2() * 0.5 * XMVector3Cross(e_ij, connection->n_i1(1)) + MTB;
			XMVECTOR m_ji = B2() * 0.5 * XMVector3Cross(e_ij, connection->n_i2(1)) - MTB;

			connection->p1()->Force() += f_ij;
			connection->p2()->Force() -= f_ij;
			connection->p1()->Moment() += m_ij;
			connection->p2()->Moment() += m_ji;
		}

		for (auto another_entity : m_entities)
			if (another_entity != entity)
				for (auto particle : entity->Particles())
					for (auto another_particle : another_entity->Particles())
					{
						float r = GeometricObject::DistanceIJ(particle, another_particle);
						float f = LennardJones(r);
						DirectX::XMVECTOR e_ij = (another_particle->VectorPosition() - particle->VectorPosition()) / r;
						particle->Force() += f * e_ij;
					}

		for (auto particle : entity->Particles())
		{
			XMFLOAT3 ANG_VEL_3{ 0.f, 0.f, 0.f };
			XMFLOAT4 QUAT{ 0.f, 0.f, 0.f , 0.f };

			auto v_new = particle->VectorVelocity() + vis * particle->VectorVelocity() + (!particle->m_isFixed) * particle->Force() / m * dt;
			auto r_new = particle->VectorPosition() + v_new * dt;
			auto av_new = particle->VectorAngularVelocity() + (!particle->m_isFixed) * particle->Moment() / J * dt;

			//auto av_new_rot = XMQuaternionMultiply(XMQuaternionInverse(particle->VectorQuaternion()), XMQuaternionMultiply(particle->VectorQuaternion(), av_new));

			XMStoreFloat3(&ANG_VEL_3, av_new);
			XMFLOAT4 ANG_VEL_4(ANG_VEL_3.x, ANG_VEL_3.y, ANG_VEL_3.z, 0.f);
			auto q_new = particle->VectorQuaternion() + 0.5 * XMQuaternionMultiply(XMLoadFloat4(&ANG_VEL_4), particle->VectorQuaternion()) * dt;


			particle->Velocity(v_new);
			particle->AngularVelocity(av_new);
			particle->Position(r_new);
			particle->Quaternion(q_new / XMQuaternionLength(q_new));
		}
		for (auto connection : entity->Connections())
			connection->Update();
		for (auto particle : entity->Particles())
			particle->ResetForcesAndMoments();
	}
	
}

