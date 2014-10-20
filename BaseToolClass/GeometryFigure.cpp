#include "GeometryFigure.h"

using namespace CGGToolKit;


const point& Line3D::origin() const
{
	return p;
}

const vec& Line3D::direction() const
{
	return d;
}

Plane::Plane(const point& pointA, const point& pointB, const point& pointC)
{
	vec edge1 = pointB - pointA;
	vec edge2 = pointC - pointA;
	nor = edge1 CROSS edge2;
	normalize(nor);
	vec v(pointA);
	distance = -(v DOT nor);
}