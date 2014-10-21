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
private:
	void removeEdges(const vector<Plane>& planes, vector<Pair>& edgeFilter, bool isKeep);
};