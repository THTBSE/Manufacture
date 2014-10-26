#pragma once
#include "../trimesh/include/TriMesh.h"
#include "../BaseToolClass/CGGToolkit.h"
#include <vector>
#include <set>
#include <queue>
using std::set;
using std::vector;
using std::queue;
using namespace CGGToolKit;

typedef std::pair<int, int> Pair;
typedef std::set<Pair>::iterator EdgeIter;

class featureEdges
{
public:
	featureEdges(TriMesh* mesh = NULL, float Thr = (M_PIf / 6)) :objMesh(mesh), diheralThr(Thr)
	{

	}
	void getBaseEdges();
	void setBasePlane(const Plane& plane) { standardPlane = plane; }
	void setMesh(TriMesh* mesh) { objMesh = mesh; }
	void addConstraintPlane(const Plane& plane, bool isIncluded);
	const vector<int>& outputBorderline(int k) const;
	virtual void getContour() = 0;
	virtual void drawContour() const;
protected:
	TriMesh* objMesh;
	float diheralThr;
	vector<Plane> keepInPlane;
	vector<Plane> dumpInPlane;
	Plane standardPlane;
	vector< vector<int> > featureCircle;
	set<Pair> fEdges;

	//Linking two nodes which are not adjacent to each other,
	//the algorithm behind this function is Dijkstra shorest path.
	//The 'start' and 'end' both are vertex index of the objMesh,
	//and it return the vertices array from 'end' to 'start' but excluding these
	//two nodes.And do not warry about returning a Large vector<int>,I use 
	//std::move() :D  
	vector<int> linkNonAdjacent(int start, int end);

	//Getting triangles inside featureCirlces;
	void getTriInside();
private:
	void removeEdges(const vector<Plane>& planes, vector<Pair>& edgeFilter, bool isKeep);
	//Getting a vertex index inside a featureCircle.
	int getVertexInside(const vector<int>& circle) const;
};