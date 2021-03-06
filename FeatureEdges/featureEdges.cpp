#include "featureEdges.h"
#include "../opengl/glew.h"
#include "../BaseToolClass/dijkstrapath.h"
#include "../trimesh/include/TriMesh_algo.h"
#include <functional>

void featureEdges::getBaseEdges()
{
	assert(objMesh);
	objMesh->need_across_edge();
	int fsize = static_cast<int>(objMesh->faces.size());
	set<Pair>::iterator pIter;
	for (int i = 0; i<fsize; i++)
	{
		for (int j = 0; j<3; j++)
		{
			float diheralAng = objMesh->dihedral_m(i, j);
			if (diheralAng > diheralThr)
			{
				int v1 = std::min(objMesh->faces[i][(j + 1) % 3], objMesh->faces[i][(j + 2) % 3]);
				int v2 = std::max(objMesh->faces[i][(j + 1) % 3], objMesh->faces[i][(j + 2) % 3]);
				Pair fedge(v1, v2);
				pIter = fEdges.find(fedge);
				if (pIter == fEdges.end())
				{
					fEdges.insert(fedge);
				}
			}
		}
	}

	vector<Pair> edgeFilter(fEdges.begin(), fEdges.end());
	//Delete edges which not in 'keepInPlane' that means keep edges in certain plane
	if (!keepInPlane.empty())
	{
		removeEdges(keepInPlane, edgeFilter, true);
	}
	//Delete edges which in 'dumpInPlane' that means dump edges in certain plane 
	if (!dumpInPlane.empty())
	{
		removeEdges(dumpInPlane, edgeFilter, false);
	}
	fEdges.clear();
	std::copy(edgeFilter.begin(), edgeFilter.end(), std::inserter(fEdges, fEdges.begin()));
}


void featureEdges::addConstraintPlane(const Plane& plane, bool isIncluded)
{
	if (isIncluded)
		keepInPlane.push_back(plane);
	else
		dumpInPlane.push_back(plane);
}

void featureEdges::removeEdges(const vector<Plane>& planes, vector<Pair>& edgeFilter, bool isKeep)
{
	vector<Pair>::iterator filterIter;
	bool initialFlag = isKeep;
	filterIter = std::remove_if(edgeFilter.begin(), edgeFilter.end(), [&,this](const Pair& e)
	{
		isKeep = initialFlag;
		for (auto i = 0; i != planes.size(); ++i)
		{
			Line3D seg(objMesh->vertices[e.first], objMesh->vertices[e.second], true);
			auto ret = intersection3D::lineIntersectPlane(seg, planes[i]);
			if (std::get<0>(ret) == CGG_COINCIDE)
			{
				isKeep = !isKeep;
				break;
			}
		}
		return isKeep;
	});
	edgeFilter.erase(filterIter, edgeFilter.end());
}

const vector<int>& featureEdges::outputBorderline(int k) const
{
	assert(k<featureCircle.size());
	return featureCircle[k];
}

void featureEdges::drawContour() const
{
	if (fEdges.empty())
		return;
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	std::for_each(fEdges.begin(), fEdges.end(), [this](const Pair& e)
	{
		glVertex3fv(&objMesh->vertices[e.first][0]);
		glVertex3fv(&objMesh->vertices[e.second][0]);
	});
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
}

vector<int> featureEdges::linkNonAdjacent(int start, int end)
{
	DijkstraPath linker(objMesh, start, end);
	linker.RunDijkstra();
	return std::move(linker.output());
}

int featureEdges::getVertexInside(const vector<int>& circle) const
{
	assert(!circle.empty());

	int borderVertex = circle.front();
	std::map<double, int, std::greater<float> > angleIndex;
	objMesh->need_neighbors();

	for (auto id : objMesh->neighbors[borderVertex])
	{
		auto iter = std::find(circle.begin(), circle.end(), id);
		if (iter != circle.end())
			continue;
		vec edgeVector = objMesh->vertices[id] - objMesh->vertices[borderVertex];
		float Angle = angle(standardPlane.normal(), edgeVector);
		angleIndex.insert(make_pair(Angle, id));
	}
	assert(!angleIndex.empty());
	return angleIndex.begin()->second;
}

void featureEdges::getTriInside(stack<vector<int>>& selectedFacets)
{
	if (featureCircle.empty())
		return;
	for (size_t i = 1; i < featureCircle.size(); ++i)
	{
		int seed = getVertexInside(featureCircle[i]);
		std::vector<int> sFacets = 
			bfs_mesh_select(objMesh, featureCircle[i], seed);
		if (!sFacets.empty())
			selectedFacets.push(std::move(sFacets));
	}
}
