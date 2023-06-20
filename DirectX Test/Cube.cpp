#include "pch.h"
#include "Cube.h"


using namespace DirectX;


Cube::Cube()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Update();
}

Cube::Cube(XMFLOAT3 position)
{
    m_position = position;
    Update();
}

void Cube::Update()
{
    XMStoreFloat4x4(
        &m_modelMatrix,
        XMMatrixScaling(0.25, 0.25, 0.25) *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_position.x)
    );
}