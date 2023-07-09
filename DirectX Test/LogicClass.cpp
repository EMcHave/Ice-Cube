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
	//k = pow(10, 8);
	dt = pow(10, -1);

	int NX = 3;
	int NY = 3;
	int NZ = 3;

	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);

	XMVECTOR axis = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&n3), 0);
	for (int k = 0; k < NZ; k++)
		for (int j = 0; j < NY; j++)
			for (int i = 0; i < NX; i++)
			{
				auto cube = std::make_shared<Cube>(XMFLOAT3(i * 2, 2 * k, -j * 2), false);
				m_objects.push_back(cube);
			}
	m_objects[0]->m_isFixed = true;
	m_objects[9]->m_isFixed = true;
	m_objects[18]->m_isFixed = true;
	for (int k = 0; k < NZ; k++)
	{
		for (int j = 0; j < NY; j++)
			for (int i = 0; i < NX - 1; i++)
			{
				std::array<std::shared_ptr<Line>, 3> vectors1
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n1)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2))
				};
				std::array<std::shared_ptr<Line>, 3> vectors2
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n1)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2))
				};
				auto connection = std::make_shared<Connection>(
					m_objects[k * NX * NY + j * NX + i], m_objects[k * NX * NY + j * NX + i + 1],
					vectors1, vectors2
				);
				m_connections.push_back(connection);
			}

		for (int j = 0; j < NY - 1; j++)
			for (int i = 0; i < NX; i++)
			{
				std::array<std::shared_ptr<Line>, 3> vectors1
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n1)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2))
				};
				std::array<std::shared_ptr<Line>, 3> vectors2
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n1)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2))
				};
				auto connection = std::make_shared<Connection>(
					m_objects[k * NX * NY + j * NX + i], m_objects[k * NX * NY + (j + 1) * NX + i],
					vectors1, vectors2
				);
				m_connections.push_back(connection);
			}
	}



	for (int k = 0; k < NZ - 1; k++)
		for (int j = 0; j < NY; j++)
			for (int i = 0; i < NX; i++)
			{
				std::array<std::shared_ptr<Line>, 3> vectors1
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n2)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n1))
				};
				std::array<std::shared_ptr<Line>, 3> vectors2
				{
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n2)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), XMLoadFloat3(&n3)),
						std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -XMLoadFloat3(&n1))
				};
				auto connection = std::make_shared<Connection>(
					m_objects[k * NX * NY + j * NX + i], m_objects[(k + 1) * NX * NY + j * NX + i],
					vectors1, vectors2
				);
				m_connections.push_back(connection);
			}

	//m_objects[8]->Position(XMFLOAT3(4, 1, -4));
	m_objects[m_objects.size()-1]->Position(XMFLOAT3(4.5, 3, -4));
}

void LogicClass::TimeStep()
{
	using namespace DirectX;
	
	XMVECTOR quaternion;

	for (auto connection : m_connections)
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

		connection->p1()->Force(f_ij);
		connection->p2()->Force(-f_ij);
		connection->p1()->Moment(m_ij);
		connection->p2()->Moment(m_ji);
	}

	for (auto particle : m_objects)
	{
		XMFLOAT3 ANG_VEL_3{ 0.f, 0.f, 0.f };
		XMFLOAT4 QUAT{ 0.f, 0.f, 0.f , 0.f };

		auto v_new = particle->VectorVelocity() + (!particle->m_isFixed) * particle->Force() / m * dt;
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
	for (auto connection : m_connections)
		connection->Update();
	for (auto particle : m_objects)
		particle->ResetForcesAndMoments();
}

DirectX::XMFLOAT4X4 LogicClass::A_Matrix(DirectX::XMFLOAT4 q)
{
	return DirectX::XMFLOAT4X4(
		q.x, -q.y, -q.z, -q.w,
		q.y, q.x, -q.w, q.z,
		q.z, q.w, q.x, -q.y,
		q.w, -q.z, q.y, q.x);
}
