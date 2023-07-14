#pragma once
#include "Camera.h"
#include "Cube.h"
#include "Line.h"
#include "Entity.h"
#include "MoveLookController.h"
#include "Connection.h"

class MyRenderer;

class LogicClass
{
public:
	LogicClass();

	void Init(
		_In_ std::shared_ptr<MoveLookController> const& controller,
		_In_ std::shared_ptr<MyRenderer> const& renderer
	);
	Camera& GameCamera() { return m_camera; };
	std::vector<std::shared_ptr<Cube>> const& RenderObjects() { return m_particles; };
	std::vector<std::shared_ptr<Connection>> const& RenderVectors() { return m_connections; };
	std::shared_ptr<Cube>& Object(int i) { return m_particles.at(i); };

	void TimeStep();

	float LennardJones(float r)
	{
		float D = 0.005;
		float A = a;

		return 12 * D / a * (pow(A / r, 7) - pow(A / r, 13));
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
	
	std::shared_ptr<MoveLookController>				m_controller;
	std::shared_ptr<MyRenderer>						m_renderer;
	Camera											m_camera;
	std::vector<std::shared_ptr<Cube>>				m_particles;
	std::vector<std::shared_ptr<Entity>>			m_entities;
	std::vector<std::shared_ptr<Connection>>		m_connections;

	float dt;
	float vis;

	float m;
	float E;
	float nu;
	float J;
	float Jp;
	float G;
	float S;
	float a;
	float k;
};

