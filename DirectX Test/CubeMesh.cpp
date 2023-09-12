#include "pch.h"
#include "CubeMesh.h"
#include "ConstantBuffers.h"

using namespace DirectX;
using namespace winrt::Windows::Foundation::Numerics;

CubeMesh::CubeMesh(_In_opt_ bool isLine, _In_ winrt::com_ptr<ID3D11Device3> const& device) : MeshObject(_In_opt_ isLine)
{
    const D3D11_INPUT_ELEMENT_DESC PNTVertexLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    /*
    PNTVertex cubeVertices[] =
    {
        { float3(-0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f) }, // +Y (top face)
        { float3(0.5f, 0.5f, -0.5f), float3(1.0f, 1.0f, 0.0f) },
        { float3(0.5f, 0.5f,  0.5f), float3(1.0f, 1.0f, 1.0f) },
        { float3(-0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 1.0f) },

        { float3(-0.5f, -0.5f,  0.5f), float3(0.0f, 0.0f, 1.0f) }, // -Y (bottom face)
        { float3(0.5f, -0.5f,  0.5f), float3(1.0f, 0.0f, 1.0f) },
        { float3(0.5f, -0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f) },
        { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, 0.0f) },
    };
    */

    const float4 color = float4(0.53f, 0.81f, 0.94f, 0.0f);
    

    PNTVertex cubeVertices[] =
    {
        { float3(-0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), color }, // +Y (top face)
        { float3(0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), color },
        { float3(0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), color },
        { float3(-0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), color },

        { float3(-0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), color }, // -Y (bottom face)
        { float3(0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), color },

        { float3(0.5f,  0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), color }, // +X (right face)
        { float3(0.5f,  0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), color },
        { float3(0.5f, -0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), color },

        { float3(-0.5f,  0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), color }, // -X (left face)
        { float3(-0.5f,  0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), color },
        { float3(-0.5f, -0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), color },

        { float3(-0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color }, // +Z (front face)
        { float3(0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },
        { float3(0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },
        { float3(-0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },

        { float3(0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color }, // -Z (back face)
        { float3(-0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
    };

    unsigned short cubeIndices[] =
    {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23
    };

    m_vertexCount = ARRAYSIZE(cubeVertices);
    m_indexCount = ARRAYSIZE(cubeIndices);

    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };

    //ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    vertexBufferDesc.ByteWidth = sizeof(PNTVertex) * m_vertexCount;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    vertexBufferData.pSysMem = cubeVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;


    winrt::check_hresult(
        device->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            m_vertexBuffer.put()
        )
    );

    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    //ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    indexBufferDesc.ByteWidth = sizeof(unsigned short) * m_indexCount;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = cubeIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;


    winrt::check_hresult(
        device->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            m_indexBuffer.put()
        )
    );
}

void CubeMesh::UpdateColor(float4 color, _In_ ID3D11DeviceContext* context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    PNTVertex cubeVertices[] =
    {
        { float3(-0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), color }, // +Y (top face)
        { float3(0.5f, 0.5f, -0.5f), float3(0.0f, 1.0f, 0.0f), color },
        { float3(0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), color },
        { float3(-0.5f, 0.5f,  0.5f), float3(0.0f, 1.0f, 0.0f), color },

        { float3(-0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), color }, // -Y (bottom face)
        { float3(0.5f, -0.5f,  0.5f), float3(0.0f, -1.0f, 0.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, -1.0f, 0.0f), color },

        { float3(0.5f,  0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), color }, // +X (right face)
        { float3(0.5f,  0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(1.0f, 0.0f, 0.0f), color },
        { float3(0.5f, -0.5f,  0.5f), float3(1.0f, 0.0f, 0.0f), color },

        { float3(-0.5f,  0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), color }, // -X (left face)
        { float3(-0.5f,  0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), color },
        { float3(-0.5f, -0.5f,  0.5f), float3(-1.0f, 0.0f, 0.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(-1.0f, 0.0f, 0.0f), color },

        { float3(-0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color }, // +Z (front face)
        { float3(0.5f,  0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },
        { float3(0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },
        { float3(-0.5f, -0.5f, 0.5f), float3(0.0f, 0.0f, 1.0f), color },

        { float3(0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color }, // -Z (back face)
        { float3(-0.5f,  0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
        { float3(-0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
        { float3(0.5f, -0.5f, -0.5f), float3(0.0f, 0.0f, -1.0f), color },
    };

    context->Map(m_vertexBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, cubeVertices, sizeof(cubeVertices));
    context->Unmap(m_vertexBuffer.get(), 0);
}
