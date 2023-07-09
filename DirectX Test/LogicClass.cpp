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
	m_connections = std::vector<std::shared_ptr<Connection>>();

	E = 20;
	G = 20;
	nu = 0.3;
	m = 1;
	a = 2;
	J = 1;
	Jp = 5 * pow(10, -10);
	S = 0.0025;
	k = pow(10, 8);
	dt = pow(10, -1);

	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);
	XMVECTOR axis = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&n3), 0);

	XMVECTOR rotated = XMVector3Rotate(XMLoadFloat3(&n3), axis);

	std::array<std::shared_ptr<Line>, 3> vectors1 
	{
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n1)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
		std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2))
	};
	std::array<std::shared_ptr<Line>, 3> vectors2
	{
		std::make_shared<Line>(Axis::W, XMFLOAT3(2, 0, 0), -XMLoadFloat3(&n1), axis),
		std::make_shared<Line>(Axis::W, XMFLOAT3(2, 0, 0), XMLoadFloat3(&n3), axis),
		std::make_shared<Line>(Axis::W, XMFLOAT3(2, 0, 0), XMLoadFloat3(&n2), axis)
	};
	

	auto cube1 = std::make_shared<Cube>(XMFLOAT3(0, 0, 0), vectors1, true);
	auto cube2 = std::make_shared<Cube>(XMFLOAT3(2, 0.5, 0), axis, vectors2, false);
	m_objects.push_back(cube1);
	m_objects.push_back(cube2);

	m_connections.push_back(
		std::make_shared<Connection>(
			cube1, cube2, vectors1, vectors2
		)
	);
}

void LogicClass::TimeStep()
{
	using namespace DirectX;
	
	for (auto particle : m_objects)
	{
		if (!particle->m_isFixed)
		{
			XMFLOAT3 F_f{ 0.f, 0.f, 0.f };
			XMFLOAT3 M_f{ 0.f, 0.f, 0.f };
			XMVECTOR F = XMLoadFloat3(&F_f);
			XMVECTOR M = XMLoadFloat3(&M_f);

			for (auto& connection : m_objects)
				if (particle != connection)
				{
					float dist = GeometricObject::DistanceIJ(particle, connection);
					XMVECTOR r_ij = -particle->VectorPosition() + connection->VectorPosition();
					XMVECTOR e_ij = r_ij / dist;
					XMVECTOR f_ij = B1() * (dist - a) * e_ij +
						B2() / 2 / dist * (connection->n_i(1) - particle->n_i(1) - XMVector3Dot(e_ij, connection->n_i(1) - particle->n_i(1)) * e_ij);
					XMVECTOR MTB = B3() * XMVector3Cross(connection->n_i(1), particle->n_i(1)) - B4() * 0.5 * (XMVector3Cross(connection->n_i(2), particle->n_i(2)) + XMVector3Cross(connection->n_i(3), particle->n_i(3)));
					XMVECTOR m_ij = -B2() * 0.5 * XMVector3Cross(e_ij, particle->n_i(1));
					M += m_ij + MTB;
					F += f_ij;
				}

			XMFLOAT3 ANG_VEL_3{ 0.f, 0.f, 0.f };
			XMFLOAT4 QUAT{ 0.f, 0.f, 0.f , 0.f };

			auto v_new = particle->VectorVelocity() + (!particle->m_isFixed) * F / m * dt;
			auto r_new = particle->VectorPosition() + v_new * dt;
			auto av_new = particle->VectorAngularVelocity() + (!particle->m_isFixed) * M / J * dt;
			//auto av_new_rot = XMQuaternionMultiply(XMQuaternionInverse(particle->VectorQuaternion()), XMQuaternionMultiply(particle->VectorQuaternion(), av_new));

			XMStoreFloat3(&ANG_VEL_3, av_new);
			XMFLOAT4 ANG_VEL_4(ANG_VEL_3.x, ANG_VEL_3.y, ANG_VEL_3.z, 0.f);
			auto q_new = particle->VectorQuaternion() + 0.5 * XMQuaternionMultiply(XMLoadFloat4(&ANG_VEL_4), particle->VectorQuaternion()) * dt;

			particle->Velocity(v_new);
			particle->AngularVelocity(av_new);
			particle->Position(r_new);
			particle->Quaternion(q_new / XMQuaternionLength(q_new));
		}
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
