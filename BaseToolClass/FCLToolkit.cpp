#include "FCLToolkit.h"
#include "../trimesh/include/KDtree.h"

int vfclToolkit::PointOnMesh(const TriMesh* mesh,const point& p)
{
	if (!mesh)
		return -1;
	
	float* v0 = const_cast<float*>(&mesh->vertices[0][0]);
	int vn = static_cast<int>(mesh->vertices.size());
	KDtree* kd = new KDtree(v0,vn);
	const float* match = kd->closest_to_pt(&p[0],1000.0f);
	int k = -1;
	if (match)
	{
		k = (match - v0) / 3;
	}
	delete kd;
	kd = NULL;
	return k;
}