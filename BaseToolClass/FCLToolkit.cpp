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

void vfclToolkit::projectToXZPlane(vector<point>& ret, const TriMesh *mesh)
{
	if (!mesh)
		return;

	ret.clear();
	std::for_each(mesh->vertices.begin(), mesh->vertices.end(), [&](const point& pt)
	{
		ret.push_back(point(pt[0], 0.0f, pt[2]));
	});

	std::sort(ret.begin(), ret.end(), [](const point& a, const point& b)
	{
		return a[0] < b[0];
	});
}