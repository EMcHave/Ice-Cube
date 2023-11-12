#pragma once
#include "Entity.h"
class Block :
    public Entity
{
public:
    Block(
        XMFLOAT4 color,
        InitialConditions initialConditions,
        Material material,
        Behavior behavior,
        XMFLOAT4 blockSize,
        XMFLOAT4 initOrientation = XMFLOAT4(0., 0., 0., 1.)
    );
    void CreateOrthogonalMesh();
    void CreateTriangularMesh1D(XMVECTOR);
    const std::shared_ptr<Cube> Particle(int i, int j, int k) override;
private:
    void CreateConnections() override;
    void TriangConnections();
    void OrthogonalConnections();
};

