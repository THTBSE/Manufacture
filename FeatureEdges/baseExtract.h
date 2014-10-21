#pragma once
#include "featureEdges.h"

class baseExtract :public featureEdges
{
public:
	virtual void getContour();
	virtual void drawContour() const;
protected:
	void groupFinder(EdgeIter ei, set<Pair>& group);
	bool linkAdjacent(const int pOrigin, int pbeg, set<Pair>& group, vector<int>& circle);
	vector<set<Pair> > edgeGroups;
	vector< vector<int> > unClosedCircle;
private:
	void circleFinder(set<Pair>& group);

};