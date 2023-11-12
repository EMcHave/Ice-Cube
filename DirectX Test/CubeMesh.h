#pragma once

#include "MeshObject.h"

class CubeMesh : public MeshObject
{
public:
	CubeMesh(_In_opt_ bool isLine,
			 _In_ winrt::com_ptr<ID3D11Device3> const& device,
			 DirectX::XMFLOAT4 color);
	void UpdateColor(winrt::Windows::Foundation::Numerics::float4 color,
					_In_ ID3D11DeviceContext* context) override;
};

