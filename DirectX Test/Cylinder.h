#pragma once
#include "Entity.h"
class Cylinder :
    public Entity
{
public:
    Cylinder(
        XMFLOAT4 color,
        InitialConditions initialConditions,
        Material material,
        Behavior behavior,
        XMFLOAT4 cylinderSize,
        float coneAngle
    );
    void CreateCylinderMesh(float coneAngle);
    const std::shared_ptr<Cube> Particle(int i, int j, int k) override;
private:
    void CreateConnections() override;
    float m_coneAngle;
};

