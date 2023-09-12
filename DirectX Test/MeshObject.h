#pragma once

enum Axis
{
    X,
    Y,
    Z, 
    W
};

class MeshObject
{
public:
    MeshObject(_In_opt_ bool isLine);

    virtual void Render(_In_ ID3D11DeviceContext* context);
    virtual void UpdateColor(winrt::Windows::Foundation::Numerics::float4 color, _In_ ID3D11DeviceContext* context) = 0;
protected:
    bool                          m_isLine;
    int                           m_vertexCount;
    int                           m_indexCount;
    winrt::com_ptr<ID3D11Buffer>  m_vertexBuffer;
    winrt::com_ptr<ID3D11Buffer>  m_indexBuffer;

};

