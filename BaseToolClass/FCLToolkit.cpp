#include "FCLToolkit.h"
#include "../trimesh/include/KDtree.h"
#include "../trimesh/include/TriMesh_algo.h"
#include <algorithm>

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

int vfclToolkit::GetFacetByPoint(const TriMesh* mesh, const point& p)
{
	assert(mesh);

	int nearestPIndex = PointOnMesh(mesh, p);
	if (nearestPIndex == -1)
		return -1;
	const std::vector<int> &adjacent = mesh->adjacentfaces[nearestPIndex];
	if (adjacent.empty())
		return -1;

	typedef std::pair<double, int> pairDI;
	std::vector<pairDI> distIndex;
	for (auto fIndex : adjacent)
	{
		point c = closest_on_face(mesh, fIndex, p);
		double tmpdist = static_cast<double>(dist2(c, p));
		distIndex.push_back(pairDI(tmpdist, fIndex));
	}

	auto iter = std::min_element(distIndex.begin(),distIndex.end(),
		[](const pairDI& lhs, const pairDI& rhs)
	{
		return lhs.first < rhs.first;
	});

	return iter->second;
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