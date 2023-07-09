#include "pch.h"
#include "Connection.h"

Connection::Connection(
	std::shared_ptr<Cube> p1,
	std::shared_ptr<Cube> p2,
	std::array<std::shared_ptr<Line>, 3> v1,
	std::array<std::shared_ptr<Line>, 3> v2)
	: m_particle1(p1),
	m_particle2(p2),
	m_vectors1(v1),
	m_vectors2(v2)
{
}

void Connection::Update()
{
	for (std::shared_ptr<Line> vec : m_vectors1)
	{
		vec->Quaternion(p1()->VectorQuaternion());
		vec->Position(p1()->VectorPosition());
	}
	for (std::shared_ptr<Line> vec : m_vectors2)
	{
		vec->Quaternion(p2()->VectorQuaternion());
		vec->Position(p2()->VectorPosition());
	}
}
