#include "pch.h"
#include "Entity.h"

Entity::Entity()
{

}

Entity::Entity(
	XMFLOAT4 color,
	InitialConditions initialConditions,
	Material material,
	Behavior behavior)
{
	m_position = initialConditions.position;
	m_velocity = initialConditions.velocity;
	m_material = material;
	m_behavior = behavior;
	m_color = color;
}

void Entity::SetInitialConditions(DirectX::XMFLOAT3 velocity)
{
	for (auto particle : m_particles)
		particle->Velocity(velocity);
}


void Entity::CreateCylinderMesh(float coneAngle)
{
	
}

void Entity::RigidRotate(XMVECTOR quat)
{
	for (auto p : m_particles)
	{
		p->Quaternion(quat);
		p->Position(XMVector3Rotate(p->VectorPosition(), quat));
	}
	for (auto c : m_connections)
		c->Update();
}

bool Entity::CheckIfConnectionExists(Cube* p1, Cube* p2)
{
	//std::scoped_lock(p1->m_mutex, p2->m_mutex);
	for (auto c : p1->Connections())
	{
		if (c != nullptr)
		{
			if (c->p1().get() == p2)
				return true;
			else if (c->p2().get() == p2)
				return true;
		}
	}
	return false;
}
