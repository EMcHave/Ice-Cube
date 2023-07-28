#pragma once
#include <array>
#include "MeshObject.h"
#include "ConstantBuffers.h"

//#include "DirectXMath.h"

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
	void AngularVelocity(DirectX::XMFLOAT3 a_velocity);
	void AngularVelocity(DirectX::XMVECTOR a_velocity);
	void AngularVelocityHalf(DirectX::XMVECTOR quaternion);
	void KineticMoment(DirectX::XMVECTOR kineticMoment);
	void Quaternion(DirectX::XMFLOAT4 quaternion);
	void Quaternion(DirectX::XMVECTOR quaternion);
	void QuaternionHalf(DirectX::XMVECTOR quaternion);
	DirectX::XMMATRIX ModelMatrix();
	DirectX::XMFLOAT3 Position();
	DirectX::XMFLOAT3 Velocity();
	DirectX::XMFLOAT3 AngularVelocity();
	DirectX::XMFLOAT4 Quaternion();
	DirectX::XMVECTOR VectorPosition();
	DirectX::XMVECTOR VectorVelocity();
	DirectX::XMVECTOR VectorAngularVelocity();
	DirectX::XMVECTOR VectorAngularVelocityHalf();
	DirectX::XMVECTOR KineticMoment();
	DirectX::XMVECTOR VectorQuaternion();
	DirectX::XMVECTOR VectorQuaternionHalf();

	bool									m_isFixed;
	static float DistanceIJ(
		std::shared_ptr<GeometricObject> const& obj_i,
		std::shared_ptr<GeometricObject> const& obj_j);

protected:
	DirectX::XMFLOAT4X4						m_modelMatrix;
	DirectX::XMMATRIX						m_initialMatrix;
	DirectX::XMFLOAT3						m_position;
	DirectX::XMFLOAT3						m_velocity;
	DirectX::XMFLOAT3						m_angularVelocity;
	DirectX::XMFLOAT3						m_angularVelocityHalf;
	DirectX::XMFLOAT3						m_kineticMoment;
	DirectX::XMFLOAT4						m_quaternion;
	DirectX::XMFLOAT4						m_quaternionHalf;
	float									m_xAngle;
	float									m_yAngle;
	float									m_zAngle;


private:
	virtual void UpdatePosition() {};

	DirectX::XMFLOAT3            m_defaultXAxis;
	DirectX::XMFLOAT3            m_defaultYAxis;
	DirectX::XMFLOAT3            m_defaultZAxis;

	std::shared_ptr<MeshObject>  m_mesh;
};

__forceinline void GeometricObject::Mesh(_In_ std::shared_ptr<MeshObject> const& mesh)
{
	m_mesh = mesh;
}

__forceinline void GeometricObject::Position(DirectX::XMFLOAT3 position)
{
	m_position = position;
	UpdatePosition();
}

__forceinline void GeometricObject::Position(DirectX::XMVECTOR position)
{
	XMStoreFloat3(&m_position, position);
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

inline void GeometricObject::AngularVelocity(DirectX::XMFLOAT3 a_velocity)
{
	m_angularVelocity = a_velocity;
}

inline void GeometricObject::AngularVelocity(DirectX::XMVECTOR a_velocity)
{
	XMStoreFloat3(&m_angularVelocity, a_velocity);
}

inline void GeometricObject::AngularVelocityHalf(DirectX::XMVECTOR a_velocity)
{
	XMStoreFloat3(&m_angularVelocityHalf, a_velocity);
}

inline void GeometricObject::KineticMoment(DirectX::XMVECTOR kineticMoment)
{
	XMStoreFloat3(&m_kineticMoment, kineticMoment);
}

inline void GeometricObject::Quaternion(DirectX::XMFLOAT4 quaternion)
{
	m_quaternion = quaternion;
}

inline void GeometricObject::Quaternion(DirectX::XMVECTOR quaternion)
{
	XMStoreFloat4(&m_quaternion, quaternion);
}

inline void GeometricObject::QuaternionHalf(DirectX::XMVECTOR quaternion)
{
	XMStoreFloat4(&m_quaternionHalf, quaternion);
}

__forceinline DirectX::XMFLOAT3 GeometricObject::Velocity()
{
	return m_velocity;
}

inline DirectX::XMVECTOR GeometricObject::VectorAngularVelocity()
{
	return DirectX::XMLoadFloat3(&m_angularVelocity);
}

inline DirectX::XMVECTOR GeometricObject::VectorAngularVelocityHalf()
{
	return XMLoadFloat3(&m_angularVelocityHalf);
}

inline DirectX::XMVECTOR GeometricObject::KineticMoment()
{
	return XMLoadFloat3(&m_kineticMoment);
}

inline DirectX::XMFLOAT3 GeometricObject::AngularVelocity()
{
	return m_angularVelocity;
}

inline DirectX::XMFLOAT4 GeometricObject::Quaternion()
{
	return m_quaternion;
}

inline DirectX::XMVECTOR GeometricObject::VectorQuaternion()
{
	return DirectX::XMLoadFloat4(&m_quaternion);
}

inline DirectX::XMVECTOR GeometricObject::VectorQuaternionHalf()
{
	return XMLoadFloat4(&m_quaternionHalf);
}

inline float GeometricObject::DistanceIJ(std::shared_ptr<GeometricObject> const& obj_i, std::shared_ptr<GeometricObject> const& obj_j)
{
	using namespace DirectX;
	XMVECTOR vectorSub = XMVectorSubtract(obj_i->VectorPosition(), obj_j->VectorPosition());
	XMVECTOR length = XMVector3Length(vectorSub);
	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	return distance;
}


__forceinline DirectX::XMVECTOR GeometricObject::VectorVelocity()
{
	return DirectX::XMLoadFloat3(&m_velocity);
}

__forceinline DirectX::XMMATRIX GeometricObject::ModelMatrix()
{
	return DirectX::XMLoadFloat4x4(&m_modelMatrix);
}