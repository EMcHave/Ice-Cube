#pragma once
#include "Camera.h"
#include "Cube.h"
#include "Line.h"
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
	std::vector<std::shared_ptr<Cube>> const& RenderObjects() { return m_objects; };
	std::shared_ptr<Cube>& Object(int i) { return m_objects.at(i); };

	void TimeStep();
	DirectX::XMFLOAT4X4 A_Matrix(DirectX::XMFLOAT4 quaternion);

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
	std::vector<std::shared_ptr<Cube>>				m_objects;
	std::vector<std::shared_ptr<Connection>>		m_connections;

	float dt;

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

