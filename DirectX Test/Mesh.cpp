#include "pch.h"
#include "Mesh.h"

Ice::Mesh::Mesh(DirectX::XMFLOAT3 size, float radius) : m_size(size), m_cellSize(radius)
{
	float dR = radius;
	NX = (int)size.x / dR;
	NY = (int)size.y / dR;
	NZ = (int)size.z / dR;

	DirectX::XMFLOAT3 sdvig(size.x * 0.5, size.y * 0.5, size.z * 0.5);

	for (int j = 0; j < NY; j++)
		for (int i = 0; i < NX; i++)
			for (int k = 0; k < NZ; k++)
				cells.push_back(std::make_shared<Ice::Cell>(i, j, k, dR, sdvig));
}

void Ice::Mesh::CleanCells()
{
	for (auto cell : cells)
		cell->Clear();
}

std::shared_ptr<Ice::Cell> Ice::Mesh::CellByPosition(DirectX::XMFLOAT3 pos)
{
	unsigned int i = (pos.x + 0.5 * m_size.x) / m_cellSize;
	unsigned int j = (pos.y + 0.5 * m_size.y) / m_cellSize;
	unsigned int k = (pos.z + 0.5 * m_size.z) / m_cellSize;

	if (i < NX && j < NY && k < NZ)
		return Cel(i, j, k);
	else
		return std::shared_ptr<Ice::Cell>(nullptr);
}

std::shared_ptr<Ice::Cell> Ice::Mesh::Cel(int i, int j, int k)
{
	return cells[NX * NZ * j + NZ * i + k];
}

Ice::Cell::Cell(int i, int j, int k, float r, DirectX::XMFLOAT3 sdvig)
{
	isEmpty = true;
	nodes[0] = Ice::Node(-sdvig.x + i * r, -sdvig.y + j * r, -sdvig.z + k * r);
	/*
	nodes[1] = Ice::Node(-sdvig.x + (i + 1) * r, -sdvig.y + j * r, -sdvig.z + k * r);
	nodes[2] = Ice::Node(-sdvig.x + i * r, -sdvig.y + j * r, -sdvig.z + (k + 1) * r);
	nodes[3] = Ice::Node(-sdvig.x + (i + 1) * r, -sdvig.y + j * r, -sdvig.z + (k + 1) * r);
	nodes[4] = Ice::Node(-sdvig.x + i * r, -sdvig.y + (j + 1) * r, -sdvig.z + k * r);
	nodes[5] = Ice::Node(-sdvig.x + (i + 1) * r, -sdvig.y + (j + 1) * r, -sdvig.z + k * r);
	nodes[6] = Ice::Node(-sdvig.x + i * r, -sdvig.y + (j + 1) * r, -sdvig.z + (k + 1) * r);
	*/
	nodes[7] = Ice::Node(-sdvig.x + (i + 1) * r, -sdvig.y + (j + 1) * r, -sdvig.z + (k + 1) * r);
}

void Ice::Cell::AddParticle(GeometricObject* p)
{
	particles.push_back(p);
	isEmpty = false;
}

void Ice::Cell::Clear()
{
	particles.clear();
	isEmpty = true;
}

Ice::Node::Node(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
