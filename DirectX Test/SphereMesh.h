#pragma once
#include "MeshObject.h"
class SphereMesh :
    public MeshObject
{
public:
    SphereMesh(
        _In_opt_ bool isLine,
        _In_ winrt::com_ptr<ID3D11Device3> const& device,
        uint16_t segments,
        DirectX::XMFLOAT4 col);
    void UpdateColor(winrt::Windows::Foundation::Numerics::float4 color, _In_ ID3D11DeviceContext* context) override;

private:
    uint16_t m_segments;
};

