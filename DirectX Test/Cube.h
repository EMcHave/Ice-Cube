#pragma once

#include "GeometricObject.h"

class Cube : public GeometricObject
{
public:
	Cube();
	Cube(DirectX::XMFLOAT3 pos);

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);

private:
	void Update();

};

__forceinline void Cube::Position(DirectX::XMFLOAT3 position)
{
	m_position = position;
	Update();
}

__forceinline void Cube::Position(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(&m_position, position);
	Update();
}
