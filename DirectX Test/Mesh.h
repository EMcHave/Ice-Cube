#pragma once
#include "GeometricObject.h"

namespace Ice
{
	struct Node
	{
		Node(){}
		Node(float x, float y, float z);
		float x;
		float y;
		float z;
	};

	struct Cell
	{
		Cell(int i, int j, int k, float r, DirectX::XMFLOAT3 sdvig);
		std::vector<GeometricObject*>& Particles() { return particles; }
		void AddParticle(GeometricObject* p);
		void Clear();
		bool isEmpty;
	private:
		std::array<Ice::Node, 8> nodes;
		std::vector<GeometricObject*> particles;
	};

	class Mesh
	{
		std::vector<std::shared_ptr<Ice::Cell>> cells;
		DirectX::XMFLOAT3 m_size;

		float m_cellSize;
		int NX;
		int NY;
		int NZ;
	public:
		Mesh(){}
		Mesh(DirectX::XMFLOAT3 size, float radius);
		
		void CleanCells();
		std::vector<std::shared_ptr<Ice::Cell>>& Cells() { return cells; }
		
		std::shared_ptr<Ice::Cell> CellByPosition(DirectX::XMFLOAT3 pos);
		std::shared_ptr<Ice::Cell> Cel(int i, int j, int k);
	};
}


