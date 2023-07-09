#include "pch.h"
#include "Cube.h"
#include "Line.h"
#include "DirectXMath.h"

using namespace DirectX;


Cube::Cube()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Update();
}

Cube::Cube(XMFLOAT3 position, std::array<std::shared_ptr<Line>, 3> rigidVectors, bool isFixed)
{
    m_isFixed = isFixed;
    m_position = position;
    m_rigidVectors = rigidVectors;
    m_initialMatrix = XMMatrixIdentity();
    Update();
}

Cube::Cube(DirectX::XMFLOAT3 position, XMVECTOR quat, std::array<std::shared_ptr<Line>, 3> rigidVectors, bool isFixed)
{
    m_isFixed = isFixed;
    m_position = position;
    m_rigidVectors = rigidVectors;
    XMStoreFloat4(&m_quaternion, quat);
    m_initialMatrix = XMMatrixIdentity();
    Update();
}

void Cube::Update()
{
    XMStoreFloat4x4(
        &m_modelMatrix, m_initialMatrix * 
        XMMatrixScaling(0.5, 0.5, 0.5) * XMMatrixRotationQuaternion(VectorQuaternion()) *
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
    );
}

void Cube::Position(DirectX::XMFLOAT3 position)
{
    m_position = position;
    //Update();
    for (std::shared_ptr<Line> vec : m_rigidVectors)
        vec->Position(position);
}

void Cube::Position(DirectX::XMVECTOR position)
{
    DirectX::XMStoreFloat3(&m_position, position);
    //Update();
    for (std::shared_ptr<Line> vec : m_rigidVectors)
        vec->Position(position);
}

void Cube::Quaternion(DirectX::XMFLOAT4 quaternion)
{
    m_quaternion = quaternion;
    for (std::shared_ptr<Line> vec : m_rigidVectors)
        vec->Quaternion(VectorQuaternion());
    Update();
}

void Cube::Quaternion(DirectX::XMVECTOR quaternion)
{
    DirectX::XMStoreFloat4(&m_quaternion, quaternion);
    for (std::shared_ptr<Line> vec : m_rigidVectors)
        vec->Quaternion(VectorQuaternion());
    Update();
}

DirectX::XMFLOAT4 Cube::Quaternion()
{
    return m_quaternion;
}
