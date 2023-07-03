#include "pch.h"
#include "Line.h"
using namespace DirectX;

Line::Line(Axis axis) : m_axis(axis)
{
	Init();
}

Line::Line(Axis axis, DirectX::XMFLOAT3 pos, DirectX::XMVECTOR orientation)
{
	m_position = pos;
	m_axis = axis;
	XMStoreFloat3(&m_vector, orientation);
	Init();
}

void Line::Update()
{ 
	if(m_axis == Axis::W)
		XMStoreFloat4x4(
			&m_modelMatrix,
			m_initialMatrix *
			XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
			XMMatrixRotationQuaternion(VectorQuaternion())
		);
		
}

void Line::Init()
{
	DirectX::XMFLOAT3 unit(1, 0, 0);
	DirectX::XMVECTOR vec = XMLoadFloat3(&unit);
	DirectX::XMFLOAT3 zero(0, 0, 0);
	DirectX::XMVECTOR vec2 = XMLoadFloat3(&zero);
	
	switch (m_axis)
	{
	case X:
		XMStoreFloat4x4(
			&m_modelMatrix,
			DirectX::XMMatrixTranslation(0.5, 0, 0)
		);
		break;
	case Y:
		XMStoreFloat4x4(
			&m_modelMatrix,
			DirectX::XMMatrixTranslation(0.5, 0, 0) * DirectX::XMMatrixRotationY(3.1415 / 2)
		);
		break;
	case Z:
		XMStoreFloat4x4(
			&m_modelMatrix,
			DirectX::XMMatrixTranslation(0.5, 0, 0) * DirectX::XMMatrixRotationZ(3.1415 / 2)
		);
	case W:
	{
		uint32_t equal;
		float dot;
		float crossLen;
		DirectX::XMVECTOR rot = XMVector3Cross(vec, VectorOrientation());
		
		XMStoreFloat(&dot, XMVector3Dot(vec, VectorOrientation()));
		XMStoreFloat(&crossLen, XMVector3Length(rot));

		float ang = atan2f(crossLen, dot);
		XMVectorEqualR(&equal, rot, vec2);
		if (equal != 128)
			m_initialMatrix =
			DirectX::XMMatrixTranslation(0, 0, 0) *
			DirectX::XMMatrixRotationAxis(rot, ang);
		else
			m_initialMatrix =
			DirectX::XMMatrixTranslation(0, 0, 0) *
			DirectX::XMMatrixRotationZ(ang);

		break;
	}
	default:
		break;
	}
}


