#include "pch.h"
#include "Cube.h"
#include "DirectXMath.h"

using namespace DirectX;


Cube::Cube()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Update();
}

Cube::Cube(XMFLOAT3 position, std::array<XMVECTOR, 3> rigidVectors, bool isFixed)
{
    m_isFixed = isFixed;
    m_position = position;
    m_rigidVectors = rigidVectors;
    Update();
}

void Cube::Update()
{
    XMStoreFloat4x4(
        &m_modelMatrix,
        XMMatrixScaling(0.5, 0.5, 0.5) *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
        XMMatrixRotationQuaternion(VectorQuaternion())
    );

    for (auto& vec : m_rigidVectors)
        vec = XMVector3Rotate(vec, VectorQuaternion());
}