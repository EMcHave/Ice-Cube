#pragma once
#include "Camera.h"
#include "Cube.h"
#include "MoveLookController.h"

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

	float B1() { return E * S / a; }
	float B2() { return 12 * k * a * E * J * S / (k * S * a * a + 24 * J * (1 + nu)); }
	float B3() { return E * J / a - B2() / 4 - B4() / 2; }
	float B4() { return G * Jp / a; }
private:
	
	std::shared_ptr<MoveLookController>				m_controller;
	std::shared_ptr<MyRenderer>						m_renderer;
	Camera											m_camera;
	std::vector<std::shared_ptr<Cube>>				m_objects;

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

