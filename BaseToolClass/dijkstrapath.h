#ifndef _DIJKSTRA_PATH_FCLLIB_H_
#define _DIJKSTRA_PATH_FCLLIB_H_
#include <queue>
#include <map>
#include <stack>
#include <vector>
#include "../trimesh/include/TriMesh.h"
using std::priority_queue;
using std::map;
using std::vector;
using std::stack;

//this algorithm instance was only written for TriMesh class .. 
class FuDijkstraNode
{
public:
	FuDijkstraNode():id(-1),pre(-1),dist(std::numeric_limits<float>::max()){}
	FuDijkstraNode(int i,int p,float d):id(i),pre(p),dist(d){}


	friend bool operator< (const FuDijkstraNode& n1,const FuDijkstraNode& n2)
	{
		return n1.dist > n2.dist;     
	}
                                  
	int    id;
	int    pre;
	float dist;
};

class DijkstraPath
{
public:
	DijkstraPath():themesh(NULL),StartNode(-1),EndNode(-1){}
	DijkstraPath(TriMesh* mesh,int S,int E):themesh(mesh),
		StartNode(S),EndNode(E){}
	int                            StartNode;
	int                            EndNode;
	TriMesh*                       themesh;
	map<int,int>                   Finish_id;
	vector<FuDijkstraNode>         FinishNode;
	void      RunDijkstra();
	vector<int>  output();
};


#endif