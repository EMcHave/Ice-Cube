#pragma once

#include "Line.h"

class Cube : public GeometricObject
{
public:
	Cube();
	Cube(DirectX::XMFLOAT3 pos, std::array<std::shared_ptr<Line>, 3> m_rigidVectors, bool);

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);

	DirectX::XMVECTOR n_i(int i) { return m_rigidVectors[i - 1]->VectorOrientation(); }
	std::shared_ptr<Line> const OrientationVector(int i) { return m_rigidVectors[i]; }


private:
	void Update();
	std::array< std::shared_ptr<Line>, 3 >	m_rigidVectors;
};

__forceinline void Cube::Position(DirectX::XMFLOAT3 position)
{
	m_position = position;
	for (std::shared_ptr<Line> vec : m_rigidVectors)
		vec->Position(position);
	Update();
}

__forceinline void Cube::Position(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(&m_position, position);
	for (std::shared_ptr<Line> vec : m_rigidVectors)
		vec->Position(position);
	Update();
}
