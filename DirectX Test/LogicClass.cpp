#include "pch.h"
#include "LogicClass.h"

LogicClass::LogicClass()
{

}

void LogicClass::Init(std::shared_ptr<MoveLookController> const& controller, std::shared_ptr<MyRenderer> const& renderer)
{
	m_controller = controller;
	m_renderer = renderer;
	m_camera.SetProjParams(70.0f * 3.1415 / 180.0f, 1.0f, 0.01f, 100.0f);

	for (int i = 0; i < 30; i++)
	{
		auto cube = std::make_shared<Cube>();
		m_objects.push_back(cube);
	}
}
