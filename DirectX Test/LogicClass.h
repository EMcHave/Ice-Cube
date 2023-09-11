#pragma once
#include <ppl.h>
#include "Camera.h"
#include "Cube.h"
#include "Line.h"
#include "Entity.h"
#include "MoveLookController.h"
#include "Connection.h"
#include "Mesh.h"

#define pidiv4 DirectX::XM_PIDIV4;
#define ALLCELLS(i, j, k) for (int k = 1; k < m_mesh.Size().z - 1; k++) for(int j = 1; j < m_mesh.Size().y - 1; j++) for(int i = 1; i < m_mesh.Size().x - 1; i++)
#define CELLSAROUND(i, j, k, e1, e2, e3) for(int e3 = k - 1; e3 <= k + 1; e3++) for(int e2 = j - 1; e2 <= j + 1; e2++) for(int e1 = i - 1; e1 <= i + 1; e1++)

class MyRenderer;

enum class InteractionModel
{
	MeshBased,
	EachToEach
};

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
	void AnimationStep(int i);

	void DT(float dt) { this->dt = dt; }
	float DT() { return dt; }

	bool IsRealTime() { return m_realTimeRender; }
	void IsRealTime(bool isRealTime) { m_realTimeRender = isRealTime; }

	void SetInteractionModel(InteractionModel intMod) { m_interactionModel = intMod; }

private:
	
	void RotationalIntegratorSymplectic(const std::shared_ptr<Cube> particle);
	void RotationalIntegratorNonSymplectic(const std::shared_ptr<Cube> particle);
	void TranslationalIntegratorLeapFrog(const std::shared_ptr<Cube> particlew);
	void EvaluateInteractionBetweenEntities(int e);
	void EvaluateInteractionInEntity(
		const std::shared_ptr<Entity> entity);
	void EvaluateMeshBasedInteraction(int i, int j, int k);
	void EvaluateArcForce(const std::shared_ptr<Entity> entity);

	InteractionModel								m_interactionModel;

	std::shared_ptr<MoveLookController>				m_controller;
	std::shared_ptr<MyRenderer>						m_renderer;
	Camera											m_camera;
	std::vector<std::shared_ptr<Cube>>				m_particles;
	std::vector<std::shared_ptr<Entity>>			m_entities;
	std::vector<std::shared_ptr<Connection>>		m_connections;
	Ice::Mesh										m_mesh;

	XMFLOAT3										m_gravity;
	XMFLOAT4										m_tempAngularVelocity4;
	XMFLOAT3										m_tempAngularVelocity3;
	
	bool											m_realTimeRender;
	bool											m_isFirstTimeStep;

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

