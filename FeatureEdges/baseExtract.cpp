#include "baseExtract.h"

void baseExtract::getContour()
{
	assert(!fEdges.empty());

	EdgeIter pIter;
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
}

void baseExtract::group_finder(EdgeIter ei, set<Pair>& group)
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

void baseExtract::circle_finder(set<Pair>& group)
{

}