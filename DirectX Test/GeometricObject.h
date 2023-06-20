#pragma once

#include "MeshObject.h"
#include "ConstantBuffers.h"

class GeometricObject
{
public:
	GeometricObject();

	void Render(
		_In_ ID3D11DeviceContext* context,
		_In_ ID3D11Buffer* primitiveConstantBuffer
	);

	void Mesh(_In_ std::shared_ptr<MeshObject> const& mesh);

	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);
	void Velocity(DirectX::XMFLOAT3 velocity);
	void Velocity(DirectX::XMVECTOR velocity);
	DirectX::XMMATRIX ModelMatrix();
	DirectX::XMFLOAT3 Position();
	DirectX::XMVECTOR VectorPosition();
	DirectX::XMVECTOR VectorVelocity();
	DirectX::XMFLOAT3 Velocity();

protected:
	DirectX::XMFLOAT4X4 m_modelMatrix;
	DirectX::XMFLOAT3   m_position;
private:
	virtual void UpdatePosition() {};

	DirectX::XMFLOAT3            m_defaultXAxis;
	DirectX::XMFLOAT3            m_defaultYAxis;
	DirectX::XMFLOAT3            m_defaultZAxis;

	DirectX::XMFLOAT3            m_velocity;
	std::shared_ptr<MeshObject>  m_mesh;
};

__forceinline void GeometricObject::Mesh(_In_ std::shared_ptr<MeshObject> const& mesh)
{
	m_mesh = mesh;
}

__forceinline void GeometricObject::Position(DirectX::XMFLOAT3 position)
{
	m_position = position;
	// Update any internal states that are dependent on the position.
	// UpdatePosition is a virtual function that is specific to the derived class.
	UpdatePosition();
}

__forceinline void GeometricObject::Position(DirectX::XMVECTOR position)
{
	XMStoreFloat3(&m_position, position);
	// Update any internal states that are dependent on the position.
	// UpdatePosition is a virtual function that is specific to the derived class.
	UpdatePosition();
}

__forceinline DirectX::XMFLOAT3 GeometricObject::Position()
{
	return m_position;
}

__forceinline DirectX::XMVECTOR GeometricObject::VectorPosition()
{
	return DirectX::XMLoadFloat3(&m_position);
}

__forceinline void GeometricObject::Velocity(DirectX::XMFLOAT3 velocity)
{
	m_velocity = velocity;
}

__forceinline void GeometricObject::Velocity(DirectX::XMVECTOR velocity)
{
	XMStoreFloat3(&m_velocity, velocity);
}

__forceinline DirectX::XMFLOAT3 GeometricObject::Velocity()
{
	return m_velocity;
}

__forceinline DirectX::XMVECTOR GeometricObject::VectorVelocity()
{
	return DirectX::XMLoadFloat3(&m_velocity);
}

__forceinline DirectX::XMMATRIX GeometricObject::ModelMatrix()
{
	return DirectX::XMLoadFloat4x4(&m_modelMatrix);
}