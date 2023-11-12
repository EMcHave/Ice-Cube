#pragma once
#include "Cube.h"
#include "Connection.h"
#include <random>

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
	const float criticalDeformationPercent = 5;
	Entity();
	Entity(
		DirectX::XMFLOAT4 color,
		InitialConditions initialConditions,
		Material material,
		Behavior behavior);
	virtual ~Entity(){}
	void SetInitialConditions(DirectX::XMFLOAT3 velocity);

	const std::vector<std::shared_ptr<Cube>>& Particles() { return m_particles; }
	std::vector<std::shared_ptr<Cube>>& ContactParticles() { return m_contactParticles; }
	std::vector<std::shared_ptr<Cube>>& BrokenParticles() { return m_brokenParticles; }
	inline std::vector<std::shared_ptr<Connection>>& Connections() { return m_connections; }

	DirectX::XMFLOAT4 Size() { return m_size; }
	virtual const std::shared_ptr<Cube> Particle(int i, int j, int k) = 0;


	void CreateCylinderMesh(float coneAngle);

	void RigidRotate(XMVECTOR);

	bool static CheckIfConnectionExists(Cube* p1, Cube* p2);

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
protected:
	virtual void CreateConnections() = 0;
	std::vector<std::shared_ptr<Connection>>	m_connections;
	std::vector<std::shared_ptr<Cube>>			m_particles;
	std::vector<std::shared_ptr<Cube>>			m_contactParticles;
	std::vector<std::shared_ptr<Cube>>			m_brokenParticles;

	DirectX::XMFLOAT4							m_size;
	DirectX::XMFLOAT3							m_position;
	DirectX::XMFLOAT3							m_velocity;
	DirectX::XMFLOAT4							m_color;

	Material									m_material;
	Behavior									m_behavior;
};

