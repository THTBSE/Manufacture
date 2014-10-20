#pragma once
#include "featureEdges.h"

class baseExtract :public featureEdges
{
public:
	virtual void getContour();

protected:
	void group_finder(EdgeIter ei, set<Pair>& group);
	vector<set<Pair> > edgeGroups;
	vector< vector<int> > unClosedCircle;
private:
	void circle_finder(set<Pair>& group);
};