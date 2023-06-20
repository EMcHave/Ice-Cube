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
	int a;
	Camera& GameCamera() { return m_camera; };
	std::vector<std::shared_ptr<Cube>> const& RenderObjects() { return m_objects; };
	std::shared_ptr<Cube>& Object(int i) { return m_objects.at(i); };
private:
	
	std::shared_ptr<MoveLookController>				m_controller;
	std::shared_ptr<MyRenderer>						m_renderer;
	Camera											m_camera;
	std::vector<std::shared_ptr<Cube>>	m_objects;
};

