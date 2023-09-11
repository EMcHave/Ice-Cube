#pragma once
#include "Cube.h"
#include "Connection.h"
#include <random>

enum EntityType
{
	PlaneTriangle,
	PlaneQuad,
	SolidQuad,
	Cylinder,
	Sphere
};

enum Behavior
{
	Flexible, 
	Rigid
};

struct InitialConditions
{
	XMFLOAT3 position;
	XMFLOAT3 velocity;
};

struct Material
{
	float E;
	float G;
	float nu;
	float rho;
};

class Entity
{
public:
	const float criticalDeformation = 0.05;
	Entity();
	Entity(
		DirectX::XMFLOAT4 size,
		InitialConditions initialConditions,
		Material material,
		EntityType type, 
		Behavior behavior);
	void SetInitialConditions(DirectX::XMFLOAT3 velocity);
	float V() { return m_size.w * (m_size.x - 1) * (m_size.y - 1) * (m_size.z - 1); }

	const std::vector<std::shared_ptr<Cube>>& Particles() { return m_particles; }
	std::vector<std::shared_ptr<Cube>>& ContactParticles() { return m_contactParticles; }
	std::vector<std::shared_ptr<Cube>>& BrokenParticles() { return m_brokenParticles; }
	std::vector<std::shared_ptr<Connection>>& Connections() { return m_connections; }
	const std::shared_ptr<Cube> Particle(int i, int j, int k) 
	{ return m_particles.at(k * m_size.x * m_size.y + j * m_size.x + i); }

	void CreateOrthogonalMesh();
	void CreateTriangularMesh1D();
	void CreateCylinderMesh(float coneAngle);

	void RigidRotate(XMVECTOR, float);

	bool static CheckIfConnectionExists(Cube* p1, Cube* p2);
	bool CheckBreak(float dist, std::shared_ptr<Connection> c);

	DirectX::XMFLOAT4 Size() { return m_size; }
	Behavior GetBehavior() { return m_behavior; }

	static float LennardJones(float r, float A, float D)
	{

		if (r > A)
			return 0;
		else if (r <= A && r >= 0.9 * A)
			return 12 * D / A * (pow(A / r, 7) - pow(A / r, 13));
		else
			return LennardJones(0.9 * A, r, D);
			
	}

	float B1()
	{
		//return E * S / a; 
		//return (1 - nu) / (1 + nu) / (1 - 2 * nu) + E * S / a;
		return 1;
	}
	float B2() {
		//return 12 * k * a * E * J * S / (k * S * a * a + 24 * J * (1 + nu)); 
		//return G * S * a;
		return 1;
	}
	float B3()
	{
		//return (1 - nu) / (1 + nu) / (1 - 2 * nu) + E * J / a - B2() / 4 - B4() / 2;
		//return E * J / a - B2() / 4 - B4() / 2; 
		return 1;
	}
	float B4()
	{
		//return G * Jp / a;
		return 1;
	}
private:
	std::vector<std::shared_ptr<Connection>>	m_connections;
	std::vector<std::shared_ptr<Cube>>			m_particles;
	std::vector<std::shared_ptr<Cube>>			m_contactParticles;
	std::vector<std::shared_ptr<Cube>>			m_brokenParticles;



	DirectX::XMFLOAT4							m_size;
	DirectX::XMFLOAT3							m_position;

	EntityType									m_entityType;
	Material									m_material;
	Behavior									m_behavior;
};

