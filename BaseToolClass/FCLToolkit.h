#ifndef _FCLTOOLKIT_H_
#define _FCLTOOLKIT_H_
#include "../trimesh/include/TriMesh.h"

class vfclToolkit
{
public:
	static int PointOnMesh(const TriMesh* mesh, const point& p);

	static int GetFacetByPoint(const TriMesh* mesh, const point& p);
	//This class method project all vertices to XZ plane and sort them as x value
	//ascending.It's a temporary class method , I will modify or delete it later.
	//2014.10.14 by vfcl 
	static void projectToXZPlane(vector<point>& ret, const TriMesh *mesh);
};


#endif