#ifndef _FCLTOOLKIT_H_
#define _FCLTOOLKIT_H_
#include "../trimesh/include/TriMesh.h"

class vfclToolkit
{
public:
	static int PointOnMesh(const TriMesh* mesh, const point& p);
};


#endif