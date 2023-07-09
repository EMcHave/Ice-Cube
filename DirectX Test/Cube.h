#pragma once

#include "Line.h"
using namespace DirectX;
class Cube : public GeometricObject
{
public:
	Cube();
	Cube(DirectX::XMFLOAT3 pos, bool);
	Cube(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR quat, bool);

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);
	void Quaternion(DirectX::XMFLOAT4 quaternion);
	void Quaternion(DirectX::XMVECTOR quaternion);
	DirectX::XMFLOAT4 Quaternion();

	void Force(DirectX::XMVECTOR f) 
	{ 
		m_force += f;
	}
	DirectX::XMVECTOR Force() { return m_force; }

	void Moment(DirectX::XMVECTOR m) 
	{ 
		m_moment += m;
	}

	void ResetForcesAndMoments()
	{
		m_force = XMVectorZero();
		m_moment = XMVectorZero();
	}
	DirectX::XMVECTOR Moment() { return m_moment; }
private:
	void Update();
	
	DirectX::XMVECTOR						m_force;
	DirectX::XMVECTOR						m_moment;
};

