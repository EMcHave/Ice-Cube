#include "pch.h"
#include "SphereMesh.h"
#include "ConstantBuffers.h"

SphereMesh::SphereMesh(
    _In_opt_ bool isLine,
    _In_ winrt::com_ptr<ID3D11Device3> const& device,
    uint16_t segments,
    DirectX::XMFLOAT4 col)
    : MeshObject(_In_opt_ isLine)
{
    using namespace DirectX;
    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
    D3D11_SUBRESOURCE_DATA vertexBufferDat = { 0 };
    m_segments = segments;

    uint16_t slices = segments / 2;
    uint16_t numVertices = (slices + 1) * (segments + 1) + 1;
    uint16_t numIndices = slices * segments * 3 * 2;

    std::vector<PNTVertex> point(numVertices);
    std::vector<uint16_t> index(numIndices);

    float4 color;
    color.w = col.w;
    color.x = col.x;
    color.y = col.y;
    color.z = col.z;

    // To make the texture look right on the top and bottom of the sphere
    // each slice will have 'segments + 1' vertices. The top and bottom
    // vertices will all be coincident, but have different U texture cooordinates.
    uint16_t p = 0;
    for (uint16_t a = 0; a <= slices; a++)
    {
        float angle1 = static_cast<float>(a) / static_cast<float>(slices) * XM_PI;
        float z = cos(angle1);
        float r = sin(angle1);
        for (uint16_t b = 0; b <= segments; b++)
        {
            float angle2 = static_cast<float>(b) / static_cast<float>(segments) * XM_2PI;
            point[p].pos = float3(r * cos(angle2), r * sin(angle2), z);
            point[p].normal = point[p].pos;
            point[p].color = color;
            p++;
        }
    }
    m_vertexCount = p;

    p = 0;
    for (uint16_t a = 0; a < slices; a++)
    {
        uint16_t p1 = a * (segments + 1);
        uint16_t p2 = (a + 1) * (segments + 1);

        // Generate two triangles for each segment around the slice.
        for (uint16_t b = 0; b < segments; b++)
        {
            if (a < (slices - 1))
            {
                // For all but the bottom slice add the triangle with one
                // vertex in the a slice and two vertices in the a + 1 slice.
                // Skip it for the bottom slice since the triangle would be
                // degenerate as all the vertices in the bottom slice are coincident.
                index[p] = b + p1;
                index[p + 1] = b + p2;
                index[p + 2] = b + p2 + 1;
                p = p + 3;
            }
            if (a > 0)
            {
                // For all but the top slice add the triangle with two
                // vertices in the a slice and one vertex in the a + 1 slice.
                // Skip it for the top slice since the triangle would be
                // degenerate as all the vertices in the top slice are coincident.
                index[p] = b + p1;
                index[p + 1] = b + p2 + 1;
                index[p + 2] = b + p1 + 1;
                p = p + 3;
            }
        }
    }
    m_indexCount = p;

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(PNTVertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDat.pSysMem = point.data();
    winrt::check_hresult(
        device->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferDat,
            m_vertexBuffer.put())
    );
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(uint16_t) * m_indexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDat.pSysMem = index.data();
    winrt::check_hresult(
        device->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferDat,
            m_indexBuffer.put())
    );
}

    void SphereMesh::UpdateColor(float4 color, _In_ ID3D11DeviceContext* context)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        uint16_t slices = m_segments / 2;
        uint16_t numVertices = (slices + 1) * (m_segments + 1) + 1;
        uint16_t numIndices = slices * m_segments * 3 * 2;

        std::vector<PNTVertex> point(numVertices);

        uint16_t p = 0;
        for (uint16_t a = 0; a <= slices; a++)
        {
            float angle1 = static_cast<float>(a) / static_cast<float>(slices) * DirectX::XM_PI;
            float z = cos(angle1);
            float r = sin(angle1);
            for (uint16_t b = 0; b <= m_segments; b++)
            {
                float angle2 = static_cast<float>(b) / static_cast<float>(m_segments) * DirectX::XM_2PI;
                point[p].pos = float3(r * cos(angle2), r * sin(angle2), z);
                point[p].normal = point[p].pos;
                point[p].color = color;
                p++;
            }
        }
        m_vertexCount = p;

        context->Map(m_vertexBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        //memcpy(mappedResource.pData, point, sizeof(cubeVertices));
        context->Unmap(m_vertexBuffer.get(), 0);
    }
