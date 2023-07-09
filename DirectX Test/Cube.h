#pragma once

#include "Line.h"

class Cube : public GeometricObject
{
public:
	Cube();
	Cube(DirectX::XMFLOAT3 pos, std::array<std::shared_ptr<Line>, 3> m_rigidVectors, bool);
	Cube(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR quat, std::array<std::shared_ptr<Line>, 3> m_rigidVectors, bool);

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);
	void Quaternion(DirectX::XMFLOAT4 quaternion);
	void Quaternion(DirectX::XMVECTOR quaternion);
	DirectX::XMFLOAT4 Quaternion();

	DirectX::XMVECTOR n_i(int i) { return m_rigidVectors[i - 1]->VectorOrientation(); }
	std::shared_ptr<Line> OrientationVector(int i) { return m_rigidVectors[i]; }

	void Force(DirectX::XMVECTOR f) { m_force = f; }
	DirectX::XMVECTOR Force() { return m_force; }

	void Moment(DirectX::XMVECTOR m) { m_moment = m; }
	DirectX::XMVECTOR Moment() { return m_moment; }
private:
	void Update();
	std::array< std::shared_ptr<Line>, 3 >	m_rigidVectors;
	DirectX::XMVECTOR						m_force;
	DirectX::XMVECTOR						m_moment;
};

