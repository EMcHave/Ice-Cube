#pragma once
#include "GeometricObject.h"

class Line : public GeometricObject
{
public:
	Line(Axis axis);
	Line(Axis axis, DirectX::XMFLOAT3 pos, DirectX::XMVECTOR orientation);


	DirectX::XMFLOAT3 Orientation() { return m_vector; }
	DirectX::XMVECTOR VectorOrientation() { return DirectX::XMLoadFloat3(&m_vector); }
	void Orientation(DirectX::XMFLOAT3 orient) { m_vector = orient; }
	void Orientation(DirectX::XMVECTOR orient) { XMStoreFloat3(&m_vector, orient); }

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);

private:
	void Update();
	void Init();
	DirectX::XMFLOAT3 m_vector;
	DirectX::XMMATRIX	m_initialMatrix;
	Axis m_axis;
};

__forceinline void Line::Position(DirectX::XMFLOAT3 position)
{
	m_position = position;
	Update();
}

__forceinline void Line::Position(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(&m_position, position);
	Update();
}

