#include "pch.h"
#include "Line.h"

Line::Line(Axis axis) : m_axis(axis)
{
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
			DirectX::XMMatrixTranslation(0.5, 0, 0) * DirectX::XMMatrixRotationY(3.1415/2)
		);
		break;
	case Z:
		XMStoreFloat4x4(
			&m_modelMatrix,
			DirectX::XMMatrixTranslation(0.5, 0, 0) * DirectX::XMMatrixRotationZ(3.1415 / 2)
		);
		break;
	default:
		break;
	}
}

