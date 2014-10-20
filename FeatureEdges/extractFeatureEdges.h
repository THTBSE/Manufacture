/*
Chilin Fu
Huaqiao University

This algorithm only for finding feature edges which can form a circle
in mesh surface.
*/
#ifndef _EXTRACTFEATUREEDGES_H_
#define _EXTRACTFEATUREEDGES_H_
#include "../trimesh/include/TriMesh.h"
#include <vector>
#include <set>
#include <queue>
using std::set;
using std::vector;
using std::queue;

typedef std::pair<int,int> Pair;
typedef std::set<Pair>::iterator EdgeIter;

class FeatureEdges
{
public:
	FeatureEdges(TriMesh* mesh=NULL,float Thr=(M_PIf/6)):objMesh(mesh),diheralThr(Thr)
	,highlightC(-1)
	{
		 
	}
	void extract_feature_edges();
	void set_mesh(TriMesh* mesh) {objMesh = mesh;}
	vector<int>& output_borderline(int k);
	void draw_borderline();
	bool set_highlight(int k);
	int  get_highlight(vector<int>& hl);

private:
	bool circle_finder(EdgeIter ei, vector<int>& pts);
	void group_finder(EdgeIter ei, set<Pair>& group);
	void circle_finder(set<Pair>& group);
	TriMesh* objMesh;
	float diheralThr;
	vector< vector<int> > featureCircle;
	set<Pair> fEdges;
	vector<set<Pair> > edgeGroups;
	int highlightC;

};

#endif