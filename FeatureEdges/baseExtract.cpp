#include "baseExtract.h"
#include "../opengl/glew.h"
#include <map>

void baseExtract::getContour()
{
	assert(!fEdges.empty());

	EdgeIter pIter;
	objMesh->need_neighbors();
	while (!fEdges.empty())
	{
		edgeGroups.push_back(set<Pair>());
		pIter = fEdges.begin();
		groupFinder(pIter, edgeGroups.back());
	}

	for (size_t i = 0; i < edgeGroups.size(); i++)
	{
		circleFinder(edgeGroups[i]);
	}

	makeClosed();
}

void baseExtract::groupFinder(EdgeIter ei, set<Pair>& group)
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
			[&, this](int endPoint2)
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

void baseExtract::circleFinder(set<Pair>& group)
{
	if (group.empty())
		return;
	set<int> vertices;
	std::for_each(group.begin(), group.end(), [&](const Pair& edge)
	{
		vertices.insert(edge.first);
		vertices.insert(edge.second);
	});

	//find the furthest vertex to the base plane 
	set<int>::iterator maxIndex;
	maxIndex = std::max_element(vertices.begin(), vertices.end(), [this](int a, int b)
	{
		return ((objMesh->vertices[a] DOT standardPlane.normal()) + standardPlane.d()) <
			((objMesh->vertices[b] DOT standardPlane.normal()) + standardPlane.d());
	});

	assert(maxIndex != vertices.end());
	int pOrigin(*maxIndex);
	EdgeIter pFinder;
	vector<int> circleA;
	circleA.push_back(pOrigin);
	int pSideA(-1), pSideB(-1);
	const float ignoreAngleThr = (160.0f / 180.0f) * M_PIf;
	vector<int> pNew;
	for (size_t i = 0; i < objMesh->neighbors[pOrigin].size(); i++)
	{
		int pNext = objMesh->neighbors[pOrigin][i];
		pFinder = group.find(Pair(std::min(pOrigin, pNext),
			std::max(pOrigin, pNext)));
		if (pFinder != group.end())
		{
			pNew.push_back(pNext);
			group.erase(pFinder);
		}
	}
	if (pNew.empty())
		return;
	std::map<float, int> canSave;
	for (size_t i = 0; i != pNew.size(); ++i)
	{
		vec v = objMesh->vertices[pNew[i]] - objMesh->vertices[pOrigin];
		float tmpAngle = angle(v, standardPlane.normal());
		if (tmpAngle < ignoreAngleThr)
		{
			canSave.insert(make_pair(tmpAngle, pNew[i]));
		}
	}
	if (canSave.empty())
		return;
	if (canSave.size() < 2)
		pSideA = canSave.begin()->second;
	else
	{
		auto mIter = canSave.begin();
		pSideA = (mIter++)->second;
		pSideB = mIter->second;
	}
	bool canClose = linkAdjacent(pOrigin,pSideA,group,circleA);

	
	if (canClose)
		featureCircle.push_back(std::move(circleA));
	else
	{
		if (pSideB != -1)
		{
			vector<int> circleB;
			canClose = linkAdjacent(pOrigin, pSideB, group, circleB);
			std::reverse(circleB.begin(), circleB.end());
			circleB.insert(circleB.end(), circleA.begin(), circleA.end());
			if (isNeighbor(circleB.front(), circleB.back()))
				featureCircle.push_back(std::move(circleB));
			else
				unClosedCircle.push_back(std::move(circleB));
		}
		else
			unClosedCircle.push_back(std::move(circleA));
	}
}

//pOrgin is the index of the furthest point, pbeg is the adjacent point of prev.
bool baseExtract::linkAdjacent(const int pOrigin, int pbeg, set<Pair>& group, vector<int>& circle)
{
	const float ignoreAngleThr = (160.0f / 180.0f) * M_PIf;
	circle.push_back(pbeg);
	int pathPoint(pbeg);
	EdgeIter pFinder;
	vec base_normal(standardPlane.normal());
	while (!objMesh->neighbors[pathPoint].empty())
	{
		vector<int> pNew;
		for (size_t i = 0; i < objMesh->neighbors[pathPoint].size(); i++)
		{
			int pNext(objMesh->neighbors[pathPoint][i]);
			if (pNext == pOrigin && circle.size() > 2)			//the second point's 
			{
				return true;
			}												//neighbor contain pbegin
															//and currCircle.size()==2
			pFinder = group.find(Pair(std::min(pathPoint, pNext),
				std::max(pathPoint, pNext)));
			if (pFinder != group.end())
			{
				pNew.push_back(pNext);
				group.erase(pFinder);
			}
		}
		if (pNew.empty())
			break;
		float minAngle = std::numeric_limits<float>::max();
		int minIndex = -1;
		for (size_t i = 0; i < pNew.size(); i++)
		{
			vec v = objMesh->vertices[pNew[i]] - objMesh->vertices[pathPoint];
			float tmpAngle = angle(v, base_normal);
			if (tmpAngle > ignoreAngleThr)
				continue;
			if (tmpAngle < minAngle)
			{
				minIndex = pNew[i];
				minAngle = tmpAngle;
			}
		}
		if (minIndex != -1)
		{
			circle.push_back(minIndex);
			pathPoint = minIndex;
		}
		else
			break;
	}
	return false;
}

void baseExtract::drawContour() const
{
//	featureEdges::drawContour();
	if (featureCircle.empty() && unClosedCircle.empty())
		return;

	glEnable(GL_COLOR_MATERIAL);
	for (size_t i = 1; i<featureCircle.size(); i++)
	{
		//if (highlightC == i)
		//	glColor3f(1.0f, 0.0f, 0.0f);
		//else
			glColor3f(1.0f, 1.0f, 0.0f);
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		for (size_t j = 0; j<featureCircle[i].size(); j++)
		{
			glVertex3fv(&objMesh->vertices[featureCircle[i][j]][0]);
		}
		glEnd();
	}
	for (size_t i = 0; i < unClosedCircle.size(); i++)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINE_STRIP);
		for (size_t j = 0; j < unClosedCircle[i].size(); j++)
		{
			glVertex3fv(&objMesh->vertices[unClosedCircle[i][j]][0]);
		}
		glEnd();
	}
	glDisable(GL_COLOR_MATERIAL);
}

void baseExtract::makeClosed()
{
	if (unClosedCircle.empty())
		return;

	for (auto &uCC : unClosedCircle)
	{
		if (uCC.empty() || uCC[0] == uCC.back())
			continue;

		int start = uCC.front(), end = uCC.back();
		vector<int> trail = linkNonAdjacent(start, end);
		if (trail.empty())
			continue;
		uCC.insert(uCC.end(), trail.begin(), trail.end());
		featureCircle.push_back(std::move(uCC));
	}
}

bool baseExtract::isNeighbor(int a, int b) const
{
	auto iter = std::find(objMesh->neighbors[a].begin(), objMesh->neighbors[a].end(), b);
	return iter != objMesh->neighbors[a].end();
}