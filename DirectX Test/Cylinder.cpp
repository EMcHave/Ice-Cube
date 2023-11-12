#include "pch.h"
#include "Cylinder.h"

Cylinder::Cylinder(
	XMFLOAT4 color,
	InitialConditions initialConditions,
	Material material,
	Behavior behavior,
	XMFLOAT4 cylinderSize,
	float coneAngle)
	: Entity(color, initialConditions, material, behavior)
{
	/*
	cylinderSize.х - радиус цилиндра
	cylinderSize.y - 0
	cylinderSize.z - высота цилиндра
	cylinderSize.w - радиус частицы
	*/
	
	int NFI = 2 * XM_PI * cylinderSize.x / cylinderSize.w;
	int NZ = cylinderSize.z / cylinderSize.w;
	m_size = XMFLOAT4(cylinderSize.x, NFI, NZ, cylinderSize.w);

	/*
	m_size.x - радиус цилиндра
	m_size.y - число частиц в сечении
	m_size.z - число частиц в высоту
	m_size.w - радиус частицы
	*/

	m_coneAngle = coneAngle;
	CreateCylinderMesh(coneAngle);
	SetInitialConditions(m_velocity);
}

void Cylinder::CreateCylinderMesh(float coneAngle)
{
	float dfi = m_size.w / m_size.x;
	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);
	bool isFixed = m_behavior == Behavior::Rigid;
	for (int k = 0; k < m_size.z; k++)
		for (int j = 0; j < m_size.y; j++)
		{
			auto cube = std::make_shared<Cube>(
				XMFLOAT3(
					(m_size.x - k * m_size.w * tanf(coneAngle)) * cosf(dfi * j) + m_position.x,
					k * m_size.w + m_position.y,
					(m_size.x - k * m_size.w * tanf(coneAngle)) * sinf(dfi * j) + m_position.z), m_color, 0.5 * m_size.w, m_size.w * 0.9, isFixed);
			XMVECTOR quat = XMQuaternionRotationNormal(XMLoadFloat3(&n2), -j * dfi);
			XMVECTOR n1_rotated = XMVector3Rotate(XMLoadFloat3(&n3), quat);
			XMVECTOR quat2 = XMQuaternionRotationNormal(n1_rotated, coneAngle);
			cube->SetInitialQuaternion(XMQuaternionMultiply(quat, quat2));
			m_particles.push_back(cube);
		}
	//if (!isFixed)
		CreateConnections();
}

const std::shared_ptr<Cube> Cylinder::Particle(int i, int j, int k)
{
	i = 0;
	return m_particles.at(k * m_size.y + j);
}

void Cylinder::CreateConnections()
{
	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);
	for (int k = 0; k < m_size.z; k++)
	{
		for (int j = 0; j < m_size.y; j++)
		{
			std::shared_ptr<Cube> p2;
			auto p1 = m_particles[k * (int)m_size.y + j];
			if (j == (int)m_size.y - 1)
				p2 = m_particles[k * (int)m_size.y];
			else
				p2 = m_particles[k * (int)m_size.y + j + 1];


			auto v1 = (p2->VectorPosition() - p1->VectorPosition()) / XMVector3Length(p2->VectorPosition() - p1->VectorPosition());
			auto v3 = XMLoadFloat3(&n2);
			auto v2 = XMVector3Cross(v1, v3);

			std::array<std::shared_ptr<Line>, 3> vectors1
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v1),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};
			std::array<std::shared_ptr<Line>, 3> vectors2
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -v1),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};

			auto connection = std::make_shared<Connection>(
				p1, p2,
				vectors1, vectors2, criticalDeformationPercent
			);

			m_connections.push_back(connection);
		}
	}

	for (int j = 0; j < m_size.y; j++)
		for (int k = 0; k < m_size.z - 1; k++)
		{
			auto p1 = m_particles[k * (int)m_size.y + j];
			auto p2 = m_particles[(k + 1) * (int)m_size.y + j];

			XMFLOAT3 z(0, k * m_size.w, 0);

			auto v1 = XMLoadFloat3(&n2);
			auto v2 = (XMLoadFloat3(&z) - p1->VectorPosition()) / XMVector3Length(XMLoadFloat3(&z) - p1->VectorPosition());
			auto v3 = XMVector3Cross(v2, v1);

			std::array<std::shared_ptr<Line>, 3> vectors1
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v1),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};
			std::array<std::shared_ptr<Line>, 3> vectors2
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -v1),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};

			auto connection = std::make_shared<Connection>(
				p1, p2,
				vectors1, vectors2, criticalDeformationPercent
			);

			m_connections.push_back(connection);
		}
}
