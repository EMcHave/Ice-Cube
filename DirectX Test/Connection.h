#pragma once
#include "Cube.h"

class Connection
{
public:
	Connection(
		std::shared_ptr<Cube> p1,
		std::shared_ptr<Cube> p2,
		std::array<std::shared_ptr<Line>, 3> v1,
		std::array<std::shared_ptr<Line>, 3> v2);
private:
	std::shared_ptr<Cube> m_particle1;
	std::shared_ptr<Cube> m_particle2;

	std::array<std::shared_ptr<Line>, 3> m_vectors1;
	std::array<std::shared_ptr<Line>, 3> m_vectors2;
};

