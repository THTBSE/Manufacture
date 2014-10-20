#include "featureEdges.h"

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
}

inline
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
	filterIter = std::remove_if(edgeFilter.begin(), edgeFilter.end(), [&isKeep,this](const Pair& e)
	{
		for (auto i = 0; i != keepInPlane.size(); ++i)
		{
			Line3D seg(objMesh->vertices[e.first], objMesh->vertices[e.second], true);
			auto ret = intersection3D::lineIntersectPlane(seg, keepInPlane[i]);
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