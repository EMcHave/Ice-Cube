#include "pch.h"
#include "LogicClass.h"

using namespace DirectX;
LogicClass::LogicClass()
{

}

void LogicClass::Init(std::shared_ptr<MoveLookController> const& controller, std::shared_ptr<MyRenderer> const& renderer)
{
	m_controller = controller;
	m_renderer = renderer;
	m_camera.SetProjParams(70.0f * 3.1415 / 180.0f, 1.0f, 0.01f, 100.0f);

	E = 1000000;
	G = 50000;
	nu = 0.3;
	m = 1;
	a = 2;
	J = 0.1;
	Jp = pow(10, -4);
	S = 0.25;
	k = 20;
	dt = pow(10, -5);

	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);

	std::array<std::shared_ptr<Line>, 3> vectors1 
	{
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n1)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n2)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3))
	};
	std::array<std::shared_ptr<Line>, 3> vectors2
	{
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n1)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n2)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3))
	};


	auto cube1 = std::make_shared<Cube>(XMFLOAT3(0, 0, 0), vectors1, true);
	auto cube2 = std::make_shared<Cube>(XMFLOAT3(2.5, 0.2, 0), vectors2, false);
	m_objects.push_back(cube1);
	m_objects.push_back(cube2);

	for (int i = 0; i < 30; i++)
	{
		//auto cube = std::make_shared<Cube>();
		//m_objects.push_back(cube);
	}
}

void LogicClass::TimeStep()
{
	
	for (auto& particle : m_objects)
	{
		XMFLOAT3 F_f{ 0.f, 0.f, 0.f };
		XMFLOAT3 M_f{ 0.f, 0.f, 0.f };
		XMVECTOR F = XMLoadFloat3(&F_f);
		XMVECTOR M = XMLoadFloat3(&M_f);

		for(auto& connection : m_objects)
			if (particle != connection)
			{
				float dist = GeometricObject::DistanceIJ(particle, connection);
				XMVECTOR r_ij = - particle->VectorPosition() + connection->VectorPosition();
				XMVECTOR e_ij = r_ij / dist;
				XMVECTOR f_ij = B1() * (dist - a) * r_ij +
					B2() / 2 / dist * (connection->n_i(1) - particle->n_i(1) - (XMVector3Dot(e_ij, connection->n_i(1)) - XMVector3Dot(e_ij, particle->n_i(1))) * e_ij);
				XMVECTOR MTB = B3() * XMVector3Cross(connection->n_i(1), particle->n_i(1)) - B4() / 2 * (XMVector3Cross(connection->n_i(2), particle->n_i(2)) + XMVector3Cross(connection->n_i(3), particle->n_i(3)));
				XMVECTOR m_ij = -B2() / 2 * XMVector3Cross(e_ij, particle->n_i(1)) + MTB;
				F += f_ij;
				M += m_ij;
			}

		XMFLOAT3 ANG_VEL_3{ 0.f, 0.f, 0.f };

		
		auto dV = particle->VectorVelocity() + (!particle->m_isFixed) * dt * F / m;
		auto dR = particle->VectorPosition() + particle->VectorVelocity() * dt;
		auto dAV = particle->VectorAngularVelocity() + (!particle->m_isFixed) *  M / J * dt;
		XMStoreFloat3(&ANG_VEL_3, dAV);

		XMFLOAT4 ANG_VEL_4(ANG_VEL_3.x, ANG_VEL_3.y, ANG_VEL_3.z, 0.f);

		auto dq = particle->VectorQuaternion() + dt * 0.5 * DirectX::XMQuaternionMultiply(particle->VectorQuaternion(), XMLoadFloat4(&ANG_VEL_4));

		particle->Velocity(dV);
		particle->AngularVelocity(dAV);
		particle->Position(dR);
		particle->Quaternion(dq / XMQuaternionLength(dq));
		
	}
}

DirectX::XMFLOAT4X4 LogicClass::A_Matrix(DirectX::XMFLOAT4 q)
{
	return DirectX::XMFLOAT4X4(
		q.x, -q.y, -q.z, -q.w,
		q.y, q.x, -q.w, q.z,
		q.z, q.w, q.x, -q.y,
		q.w, -q.z, q.y, q.x);
}
