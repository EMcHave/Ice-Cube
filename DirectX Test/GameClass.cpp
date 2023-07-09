#include "pch.h"
#include "GameClass.h"


GameClass::GameClass(std::shared_ptr<DX::DeviceResources> const& deviceResources)
	:m_deviceResources(deviceResources)
{
	m_deviceResources->RegisterDeviceNotify(this);
	m_sceneRenderer = std::make_shared<MyRenderer>(m_deviceResources);
	m_controller = std::make_shared<MoveLookController>(winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread());
	m_logic = std::make_shared<LogicClass>();
	m_logic->Init(m_controller, m_sceneRenderer);
	delta = 0;
}

GameClass::~GameClass()
{
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void GameClass::CreateWindowSizeDependentResources()
{
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

void GameClass::CreateDeviceDependentResources()
{
	m_sceneRenderer->CreateDeviceDependentResources(m_logic);
}

void GameClass::AddObject(std::shared_ptr<Cube> cube)
{
	//m_objects.push_back(cube);
}

void GameClass::Update()
{
	delta += 1;
	auto pos = m_controller->Position();
	m_controller->Update();
	m_logic->GameCamera().SetViewParams(
		m_controller->Position(),
		m_controller->Orientation(),
		DirectX::XMFLOAT3(0, 1, 0)
	);

	m_logic->TimeStep();
	//for (int i = 0; i < m_logic->RenderObjects().size(); i++)
		//m_logic->Object(i)->Position(DirectX::XMFLOAT3(sinf((i + 1) * delta), cosf((i + 1) * delta), -sinf((i + 1) * delta)));
}

bool GameClass::Render()
{
	m_sceneRenderer->Render();

	return true;
}

void GameClass::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
}

void GameClass::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources(m_logic);
	CreateWindowSizeDependentResources();
}
