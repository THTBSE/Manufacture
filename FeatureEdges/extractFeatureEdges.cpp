#include "extractFeatureEdges.h"
#include "../opengl/glew.h"
#include "../BaseToolClass/FCLToolkit.h"

void FeatureEdges::extract_feature_edges()
{
	objMesh->need_across_edge();
	int fsize = static_cast<int>(objMesh->faces.size());
	set<Pair>::iterator pIter;
	for (int i=0; i<fsize; i++)
	{
		for (int j=0; j<3; j++)
		{
			float diheralAng = objMesh->dihedral_m(i,j);
			if (diheralAng > diheralThr)
			{
				int v1 = std::min(objMesh->faces[i][(j+1)%3],objMesh->faces[i][(j+2)%3]);
				int v2 = std::max(objMesh->faces[i][(j+1)%3],objMesh->faces[i][(j+2)%3]);
		//		if (!is_in_standard(objMesh->vertices[v1]) || 
		//			!is_in_standard(objMesh->vertices[v2]))
		//			continue;
				Pair fedge(v1,v2);
				pIter = fEdges.find(fedge);
				if (pIter == fEdges.end())
				{
					fEdges.insert(fedge);
				}
			}
		}
	}
	
	objMesh->need_neighbors();
	while (!fEdges.empty())
	{
		edgeGroups.push_back(set<Pair>());
		pIter = fEdges.begin();
		group_finder(pIter, edgeGroups.back());
	}

	
	for (size_t i = 0; i < edgeGroups.size(); i++)
	{
		circle_finder(edgeGroups[i]);
	}
	
	int breakit = 1;
	/*
	vector<int> tempEdges;
	while (!fEdges.empty())
	{
		pIter = fEdges.begin();
		int pbeg = pIter->first;
		if (circle_finder(pIter, tempEdges))
		{
			featureCircle.push_back(tempEdges);
		}
		else
			featureCircle.push_back(tempEdges);
		tempEdges.clear();
	}*/
}

void FeatureEdges::group_finder(EdgeIter ei, set<Pair>& group)
{
	int pbeg1(ei->first), pbeg2(ei->second);
	group.insert(*ei);
	fEdges.erase(ei);
	EdgeIter pFinder;
	queue<int> pathPoint;
	pathPoint.push(pbeg1);
	pathPoint.push(pbeg2);

	while (!pathPoint.empty())
	{
		int endPoint1 = pathPoint.front();
		pathPoint.pop();
		std::for_each(objMesh->neighbors[endPoint1].begin(), objMesh->neighbors[endPoint1].end(),
			[&,this](int endPoint2)
		{

			Pair theEdge(std::min(endPoint1, endPoint2),
				std::max(endPoint1, endPoint2));
			if (!group.count(theEdge))
			{
				pFinder = fEdges.find(theEdge);
				if (pFinder != fEdges.end())
				{
					pathPoint.push(endPoint2);
					fEdges.erase(pFinder);
					group.insert(theEdge);
				}
			}
		});
	}
}

void FeatureEdges::circle_finder(set<Pair>& group)
{
	if (group.empty())
		return;
	set<int> vertices;
	std::for_each(group.begin(), group.end(), [&](const Pair& edge)
	{
		vertices.insert(edge.first);
		vertices.insert(edge.second);
	});

	//find the vertex which z value is max 
	set<int>::iterator maxIndex;
	maxIndex = std::max_element(vertices.begin(), vertices.end(), [this](int a, int b)
	{
		return objMesh->vertices[a][2] < objMesh->vertices[b][2];
	});

	assert(maxIndex != vertices.end());
	int pathPoint(*maxIndex);
	int pbegin(pathPoint);
	set<Pair>::iterator pFinder;
	featureCircle.push_back(vector<int>());
	vector<int>& currCircle = featureCircle.back();
	vec base_normal(0.0f, 0.0f, 1.0f);
	currCircle.push_back(pathPoint);
	while (!objMesh->neighbors[pathPoint].empty())
	{
		vector<int> pNew;
		for (size_t i = 0; i < objMesh->neighbors[pathPoint].size(); i++)
		{
			int pNext(objMesh->neighbors[pathPoint][i]);
			if (pNext == pbegin && currCircle.size() > 2)
				break;
			pFinder = group.find(Pair(std::min(pathPoint, pNext), std::max(pathPoint, pNext)));
			if (pFinder != group.end())
			{
				pNew.push_back(pNext);
				group.erase(pFinder);
			}
		}
		if (pNew.empty())
			break;
		float minAngle = std::numeric_limits<float>::max();
		int minIndex;
		for (size_t i = 0; i < pNew.size(); i++)
		{
			vec v = objMesh->vertices[pNew[i]] - objMesh->vertices[pathPoint];
			float tmpAngle = angle(v, base_normal);
			if (tmpAngle < minAngle)
			{
				minIndex = static_cast<int>(i);
				minAngle = tmpAngle;
			}
		}
		 currCircle.push_back(pNew[minIndex]);
		pathPoint = pNew[minIndex];
		if (pathPoint == pbegin)
			break;
	}

}
bool FeatureEdges::circle_finder(EdgeIter ei, vector<int>& pts)
{
	int pbeg,pprev;
	pbeg = pprev = ei->first;
	int pjoint = ei->second;
	fEdges.erase(ei);
	pts.push_back(pbeg);
	pts.push_back(pjoint);
	EdgeIter pFinder;
	while (!objMesh->neighbors[pjoint].empty())
	{
		bool found = false;
		for (size_t i=0; i<objMesh->neighbors[pjoint].size(); i++)
		{
			int pnext = objMesh->neighbors[pjoint][i];
			if (pnext == pprev)
				continue;
			pFinder = fEdges.find(Pair(std::min(pjoint,pnext),std::max(pjoint,pnext)));
			if (pFinder != fEdges.end())
			{
				if (pnext == pbeg)
				{
					fEdges.erase(pFinder);
					return true;
				}
				else
				{
					found = true;
					pts.push_back(pnext);
					pprev = pjoint;
					pjoint = pnext;
					fEdges.erase(pFinder);
					break;
				}
			}
		}
		if (!found)
		{
//			pts.clear();
			return false;
		}
	}
	return false;
}

vector<int>& FeatureEdges::output_borderline(int k)
{
	assert(k<featureCircle.size());
	return featureCircle[k];
}

void FeatureEdges::draw_borderline()
{
	if (featureCircle.empty())
		return;
	
	glEnable(GL_COLOR_MATERIAL);
	for (size_t i=1; i<featureCircle.size(); i++)
	{
		if (highlightC == i)
			glColor3f(1.0f,0.0f,0.0f);
		else
			glColor3f(1.0f,1.0f,0.0f);
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		for (size_t j=0; j<featureCircle[i].size(); j++)
		{
			glVertex3fv(&objMesh->vertices[featureCircle[i][j]][0]);
		}
		glEnd();
	}
	glDisable(GL_COLOR_MATERIAL);
	
	/*
	if (edgeGroups.empty())
		return;
	glEnable(GL_COLOR_MATERIAL);
	glLineWidth(5);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	for (size_t i = 0; i < 1; i++)
	{
		std::for_each(edgeGroups[i].begin(), edgeGroups[i].end(), [&, this](const Pair& e)
		{
			glVertex3fv(&objMesh->vertices[e.first][0]);
			glVertex3fv(&objMesh->vertices[e.second][0]);
		});
	}
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
	*/
}

//k is the point index 
bool FeatureEdges::set_highlight(int k)
{
	for (size_t i=0; i<featureCircle.size(); i++)
	{
		vector<int>::iterator it;
		it = find(featureCircle[i].begin(),featureCircle[i].end(),k);
		if (it != featureCircle[i].end())
		{
			highlightC = i;
			return true;
		}
	}
	return false;
}

int FeatureEdges::get_highlight(vector<int>& hl)
{
	if (highlightC < 0 || highlightC >= static_cast<int>(featureCircle.size()))
		return -1;

	hl.clear();
	hl.assign(featureCircle[highlightC].begin(),featureCircle[highlightC].end());
	return highlightC;
}

void FeatureEdges::get_standard_curve(const TriMesh* mesh)
{
	vfclToolkit::projectToXZPlane(standardCurve, mesh);
}

bool FeatureEdges::is_in_standard(const point& pt)
{
	//if non standardCurve , keep all.
	if (standardCurve.empty())
		return true;
	vector<point>::iterator iter;
	iter = std::lower_bound(standardCurve.begin(), standardCurve.end(), pt, []
		(const point& a, const point& b){return a[0] < b[0]; });

	if (iter == standardCurve.end())
		return false;
	const point& pp = *iter;
	if (iter == standardCurve.begin())
	{
		if (pp[0] - pt[0] > 0.2)
			return false;
	}
	
	/*
	const point& prev = *(iter - 1);
	const point& curr = *(iter);
	point zlower = std::min(curr, prev, [](const point& a, const point& b)
	{return a[2] < b[2]; });
	point zupper = std::max(curr, prev, [](const point& a, const point& b)
	{return a[2] < b[2]; });
	
	if (zlower[2] < pt[2] && pt[2] < zupper[2])
		return true;
	else
		return false;
	*/
	
//	const point& pp = *(iter);
	float d = fabs(pp[2] - pt[2]);
	if (d < 0.2f)
		return true;
	else
		return false;
}

void FeatureEdges::draw_project_curve()
{
	if (standardCurve.empty())
		return;
	
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_LINE_STRIP);
	glLineWidth(5.0f);
	std::for_each(standardCurve.begin(), standardCurve.end(), [](const point& v)
	{
		glVertex3fv(&v[0]);
	});
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
}