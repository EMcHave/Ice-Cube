#pragma once
#include <mutex>
#include "GeometricObject.h"

using namespace DirectX;
class Connection;
class Cube : public GeometricObject
{
public:
	Cube();
	Cube(DirectX::XMFLOAT3 pos, XMFLOAT4 color, float, float, bool);
	Cube(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR quat, XMFLOAT4 color, float, float, bool);

	void SetInitialQuaternion(XMVECTOR);

	using GeometricObject::Position;
	void Position(DirectX::XMFLOAT3 position);
	void Position(DirectX::XMVECTOR position);
	void Quaternion(DirectX::XMFLOAT4 quaternion);
	void Quaternion(DirectX::XMVECTOR quaternion);
	DirectX::XMFLOAT4 Quaternion();

	void RegisterConnection(Connection* con);
	void UnRegisterConnection(Connection* con);
	uint32_t NumberOfConnections() { return m_numberOfConnections; }
	std::vector<Connection*>& Connections() { return m_connections; }

	float R() { return m_radius; }

	void Force(DirectX::XMVECTOR f) { m_force = f; }
	DirectX::XMVECTOR& Force() { return m_force; }

	void Moment(DirectX::XMVECTOR m) { m_moment = m; }
	DirectX::XMVECTOR& Moment() { return m_moment; }

	bool IsContact() { return m_isContact; }
	void MakeContact() { m_isContact = true; }

	void ResetForcesAndMoments()
	{
		m_force = XMVectorZero();
		m_moment = XMVectorZero();
	}
	
	void AddTimePoint(XMVECTOR pos, XMVECTOR quat) 
	{ 
		m_positions.push_back(pos); 
		m_orientations.push_back(quat);
	}

	std::mutex								m_mutex;
	std::vector<XMVECTOR>					m_positions;
	std::vector<XMVECTOR>					m_orientations;
private:
	void Update();
	

	
	DirectX::XMVECTOR								m_force;
	DirectX::XMVECTOR								m_moment;
	uint32_t										m_numberOfConnections;
	std::vector<Connection*>						m_connections;
	float											m_radius;
	float											m_scale;
	bool											m_isContact;
};

