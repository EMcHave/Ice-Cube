#include "pch.h"
#include "Block.h"

Block::Block(
	XMFLOAT4 color,
	InitialConditions initialConditions,
	Material material,
	Behavior behavior,
	XMFLOAT4 blockSize, 
	XMFLOAT4 initOrientation)
	: Entity(color, initialConditions, material, behavior)
{
	/*
	blockSize.х - длина блока
	blockSize.y - ширина блока
	blockSize.z - высота блока
	blockSize.w - радиус частицы
	*/

	int NX = blockSize.x / blockSize.w;
	int NY = blockSize.y / blockSize.w;
	int NZ = blockSize.z / blockSize.w;
	m_size = XMFLOAT4(NX, NY, NZ, blockSize.w);

	/*
	m_size.х - число частиц в длину
	m_size.y - число частиц в ширину
	m_size.z - число частиц в высоту
	m_size.w - радиус частицы
	*/
	if (m_behavior != Behavior::Rigid)
	{
		CreateTriangularMesh1D(XMLoadFloat4(&initOrientation));
		TriangConnections();
	}
	else
	{
		CreateTriangularMesh1D(XMLoadFloat4(&initOrientation));
		//CreateOrthogonalMesh();
		OrthogonalConnections();
	}
		
	RigidRotate(XMLoadFloat4(&initOrientation));
	for (auto p : m_particles)
	{
		XMFLOAT3 newPos(0, 0, 0);
		newPos.x = p->Position().x + m_position.x;
		newPos.y = p->Position().y + m_position.y;
		newPos.z = p->Position().z + m_position.z;
		p->Position(newPos);
	}
	SetInitialConditions(initialConditions.velocity);
}

void Block::CreateOrthogonalMesh()
{
	bool isFixed = m_behavior == Behavior::Rigid;
	for (int k = 0; k < m_size.z; k++)
		for (int j = 0; j < m_size.y; j++)
			for (int i = 0; i < m_size.x; i++)
			{
				auto cube = std::make_shared<Cube>(
					XMFLOAT3(i * m_size.w, k * m_size.w, -j * m_size.w), m_color, 0.5 * m_size.w, 0.9 * m_size.w);
				m_particles.push_back(cube);
			}

	
}

void Block::CreateTriangularMesh1D(XMVECTOR initOrientation)
{
	float scale = m_size.w * 0.5;
	float radius = 0.5 * m_size.w;
	for (int j = 0; j < m_size.y; j++)
		for (int i = 0; i < m_size.x; i++)
		{
			std::shared_ptr<Cube> cube;
			if (j % 2 == 0)
			{
				cube = std::make_shared<Cube>(
					XMFLOAT3(
						i * m_size.w,
						0,
						-j * m_size.w * sinf(XM_PI / 3)), m_color, radius, scale);
			}
			else
			{
				cube = std::make_shared<Cube>(
					XMFLOAT3(
						i * m_size.w + m_size.w * cosf(XM_PI / 3),
						0,
						-j * m_size.w * sinf(XM_PI / 3)), m_color, radius, scale);
			}

			m_particles.push_back(cube);
		}
}

const std::shared_ptr<Cube> Block::Particle(int i, int j, int k)
{
	return m_particles.at(k * m_size.x * m_size.y + j * m_size.x + i);
}

void Block::CreateConnections()
{
}

void Block::TriangConnections()
{
	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);

	XMVECTOR quat60 = XMQuaternionRotationNormal(XMLoadFloat3(&n2), XM_PI / 3);
	XMVECTOR quat120 = XMQuaternionRotationNormal(XMLoadFloat3(&n2), 2 * XM_PI / 3);

	XMVECTOR rotated = XMVector3Rotate(XMLoadFloat3(&n1), quat120);

	for (int j = 0; j < m_size.y; j++)
		for (int i = 0; i < m_size.x - 1; i++)
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
				m_particles[j * m_size.x + i], m_particles[j * m_size.x + i + 1],
				vectors1, vectors2, criticalDeformationPercent
			);

			m_connections.push_back(connection);
		}

	for (int j = 0; j < m_size.y - 1; j++)
		for (int i = 0; i < m_size.x; i++)
		{
			auto v1 = (j % 2 == 0) * XMVector3Rotate(XMLoadFloat3(&n1), quat60) + (j % 2 != 0) * XMVector3Rotate(XMLoadFloat3(&n1), quat120);
			auto v2 = (j % 2 == 0) * XMVector3Rotate(XMLoadFloat3(&n3), quat60) + (j % 2 != 0) * XMVector3Rotate(XMLoadFloat3(&n3), quat120);
			auto v3 = XMLoadFloat3(&n3);

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
				m_particles[j * m_size.x + i], m_particles[(j + 1) * m_size.x + i],
				vectors1, vectors2, criticalDeformationPercent
			);
			m_connections.push_back(connection);
		}

	for (int i = 0; i < m_size.x - 1; i++)
		for (int j = 0; j < m_size.y - 1; j++)
		{
			auto v1_b = XMVector3Rotate(XMLoadFloat3(&n1), quat120);
			auto v2_b = XMVector3Rotate(XMLoadFloat3(&n3), quat120);

			auto v1_f = XMVector3Rotate(XMLoadFloat3(&n1), quat60);
			auto v2_f = XMVector3Rotate(XMLoadFloat3(&n3), quat60);

			auto v3 = XMLoadFloat3(&n3);

			std::array<std::shared_ptr<Line>, 3> vectors1_b
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v1_b),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2_b),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};
			std::array<std::shared_ptr<Line>, 3> vectors2_b
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -v1_b),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2_b),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};

			std::array<std::shared_ptr<Line>, 3> vectors1_f
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v1_f),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2_f),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};
			std::array<std::shared_ptr<Line>, 3> vectors2_f
			{
				std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), -v1_f),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v2_f),
					std::make_shared<Line>(Axis::W, XMFLOAT3(0, 0, 0), v3)
			};

			if (j % 2 == 0)
			{
				auto connection = std::make_shared<Connection>(
					m_particles[j * m_size.x + (i + 1)], m_particles[(j + 1) * m_size.x + i],
					vectors1_b, vectors2_b, criticalDeformationPercent
				);
				m_connections.push_back(connection);
			}
			else
			{
				auto connection2 = std::make_shared<Connection>(
					m_particles[j * m_size.x + i], m_particles[(j + 1) * m_size.x + (i + 1)],
					vectors1_f, vectors2_f, criticalDeformationPercent
				);
				m_connections.push_back(connection2);
			}
		}
}

void Block::OrthogonalConnections()
{
	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);
	for (int k = 0; k < m_size.z; k++)
	{
		for (int j = 0; j < m_size.y; j++)
			for (int i = 0; i < m_size.x - 1; i++)
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
					m_particles[k * m_size.x * m_size.y + j * m_size.x + i], m_particles[k * m_size.x * m_size.y + j * m_size.x + i + 1],
					vectors1, vectors2, criticalDeformationPercent
				);

				m_connections.push_back(connection);
			}

		for (int j = 0; j < m_size.y - 1; j++)
			for (int i = 0; i < m_size.x; i++)
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
					m_particles[k * m_size.x * m_size.y + j * m_size.x + i], m_particles[k * m_size.x * m_size.y + (j + 1) * m_size.x + i],
					vectors1, vectors2, criticalDeformationPercent
				);
				m_connections.push_back(connection);
			}
	}

	for (int k = 0; k < m_size.z - 1; k++)
		for (int j = 0; j < m_size.y; j++)
			for (int i = 0; i < m_size.x; i++)
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
					m_particles[k * m_size.x * m_size.y + j * m_size.x + i], m_particles[(k + 1) * m_size.x * m_size.y + j * m_size.x + i],
					vectors1, vectors2, criticalDeformationPercent
				);
				m_connections.push_back(connection);
			}
}
