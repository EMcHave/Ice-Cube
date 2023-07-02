#pragma once
#include "GeometricObject.h"

class Line : public GeometricObject
{
public:
	Line(Axis axis);
	Axis m_axis;
};

