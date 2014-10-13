#include "circleSelection.h"
#include <numeric>

void circleSelection::toScreen(TriMesh* mesh, vector<int>& vIndex, vector<vec2>& sp
	,COpenGLDC* PGLRC)
{
	if (!mesh || vIndex.empty())
		return;

	for_each(vIndex.begin(),vIndex.end(),[&](int id)
	{
		vec2 p;
		COpenglSelection::PointToScreen(PGLRC,mesh->vertices[id],p);
		sp.push_back(p);
	});
}

vec2 circleSelection::getPointWithinCircle(vector<vec2>& sp)
{
	vec2 p;
	if (sp.empty())
		return p;
	p = std::accumulate(sp.begin(),sp.end(),p);
	float t = 1.0f / static_cast<float>(sp.size());
	p = t * p;
	return p;
}