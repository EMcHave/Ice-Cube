#pragma once
#include "Cube.h"
#include "Connection.h"
class Entity
{
public:
	Entity(
		DirectX::XMFLOAT4 size,
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 constraints);
	void SetInitialConditions(DirectX::XMFLOAT3 velocity);
	float V() { return m_size.w * (m_size.x - 1) * (m_size.y - 1) * (m_size.z - 1); }

	const  std::vector<std::shared_ptr<Cube>>& Particles() { return m_particles; }
	const  std::vector<std::shared_ptr<Connection>>& Connections() { return m_connections; }
private:
	std::vector<std::shared_ptr<Connection>>	m_connections;
	std::vector<std::shared_ptr<Cube>>			m_particles;
	DirectX::XMFLOAT4							m_size;
};

