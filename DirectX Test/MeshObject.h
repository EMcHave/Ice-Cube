#pragma once
class MeshObject
{
public:
    MeshObject();

    virtual void Render(_In_ ID3D11DeviceContext* context);

protected:
    int                           m_vertexCount;
    int                           m_indexCount;
    winrt::com_ptr<ID3D11Buffer>  m_vertexBuffer;
    winrt::com_ptr<ID3D11Buffer>  m_indexBuffer;
};

