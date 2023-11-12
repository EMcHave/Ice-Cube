#include "pch.h"
#include "Connection.h"
#include <random>

Connection::Connection(
	std::shared_ptr<Cube> p1,
	std::shared_ptr<Cube> p2,
	std::array<std::shared_ptr<Line>, 3> v1,
	std::array<std::shared_ptr<Line>, 3> v2, 
	float criticalTensileStrain)
	: m_particle1(p1),
	m_particle2(p2),
	m_vectors1(v1),
	m_vectors2(v2)
{
	m_isBroken = false;
	m_criticalShearStrain = 100000;
	auto distance = p2->VectorPosition() - p1->VectorPosition();
	XMStoreFloat(&m_initialLength, XMVector3Length(distance));
 	p1->RegisterConnection(this);
	p2->RegisterConnection(this);

	const float percent = 33;

	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(0, 2 * percent);
	static std::uniform_real_distribution<> dis2(0, 4 * percent);
	m_deltaStiffness = (dis(e) - percent) * 0.01;
	float deltaStrain = (dis2(e) - 2 * percent) * 0.01;
	m_criticalTensileStrain = criticalTensileStrain * (1 + deltaStrain) * 0.01;
 }


void Connection::Update()
{
	if (!m_isBroken)
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
}

bool Connection::CheckBreak(float dist)
{
	float tensile;
	float shear;
	XMVECTOR dR = p2()->VectorPosition() - p1()->VectorPosition();
	XMVECTOR tensileVec = XMVector3Dot(dR, n_i1(1));
	XMStoreFloat(&tensile, tensileVec);
	shear = sqrt(dist * dist - tensile * tensile);
	if ((tensile - m_initialLength) / m_initialLength > m_criticalTensileStrain)
		return true;
	else if (shear > m_criticalShearStrain)
		return true;
	else
		return false;
}

void Connection::Break(std::vector<std::shared_ptr<Cube>>& contactParticles)
{
	m_isBroken = true;
	m_particle1->UnRegisterConnection(this);
	m_particle2->UnRegisterConnection(this);


	if (!m_particle1->IsContact())
	{
		contactParticles.push_back(m_particle1);
		m_particle1->MakeContact();
	}
	if (!m_particle2->IsContact())
	{
		contactParticles.push_back(m_particle2);
		m_particle2->MakeContact();
	}
}
