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
using std::set;
using std::vector;


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

	//methods below will be modified or deleted later.   2014/10/14
	void get_standard_curve(const TriMesh *mesh);
	void draw_project_curve();
	bool is_in_standard(const point& pt);

private:
	bool circle_finder(EdgeIter ei, vector<int>& pts);
	TriMesh* objMesh;
	float diheralThr;
	vector< vector<int> > featureCircle;
	set<Pair> fEdges;
	int highlightC;

	//it will be modified or deleted later.   2014/10/14
	vector<point> standardCurve;
};

#endif