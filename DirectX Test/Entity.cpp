#include "pch.h"
#include "Entity.h"

Entity::Entity(
	DirectX::XMFLOAT4 size,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 constraints)
{
	m_size = size;
	XMFLOAT3 n1(1, 0, 0);
	XMFLOAT3 n2(0, 1, 0);
	XMFLOAT3 n3(0, 0, 1);

	for (int k = 0; k < size.z; k++)
		for (int j = 0; j < size.y; j++)
			for (int i = 0; i < size.x; i++)
			{
				auto cube = std::make_shared<Cube>(XMFLOAT3(i * size.w + position.x, k * size.w + position.y, -j * size.w + position.z), false);
				m_particles.push_back(cube);
			}

	for (int k = 0; k < size.z; k++)
	{
		for (int j = 0; j < size.y; j++)
			for (int i = 0; i < size.x - 1; i++)
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
					m_particles[k * size.x * size.y + j * size.x + i], m_particles[k * size.x * size.y + j * size.x + i + 1],
					vectors1, vectors2
				);

				m_connections.push_back(connection);
			}

		for (int j = 0; j < size.y - 1; j++)
			for (int i = 0; i < size.x; i++)
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
					m_particles[k * size.x * size.y + j * size.x + i], m_particles[k * size.x * size.y + (j + 1) * size.x + i],
					vectors1, vectors2
				);
				m_connections.push_back(connection);
			}
	}

	for (int k = 0; k < size.z - 1; k++)
		for (int j = 0; j < size.y; j++)
			for (int i = 0; i < size.x; i++)
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
					m_particles[k * size.x * size.y + j * size.x + i], m_particles[(k + 1) * size.x * size.y + j * size.x + i],
					vectors1, vectors2
				);
				m_connections.push_back(connection);
			}

	SetInitialConditions(constraints);
}

void Entity::SetInitialConditions(DirectX::XMFLOAT3 velocity)
{
	for (auto particle : m_particles)
		particle->Velocity(velocity);
}
