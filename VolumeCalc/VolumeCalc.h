/*
Chilin Fu
Huaqiao University

VolumeCalc.h
Solutions for calculating the approximate volume of concaves area of 
the triangle mesh.

This method is based on the assumption that considers the mesh model
is a CAD model.So that it has apparent feature edges which we can detect
them according to the dihedral angle of their adjacent facets.
*/
#ifndef _VOLUMECALC_H_
#define _VOLUMECALC_H_
#include "../trimesh/include/TriMesh.h"
#include "../FeatureEdges/extractFeatureEdges.h"
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
using std::vector;
using std::set;
using std::queue;
using std::map;
using std::stack;

typedef std::pair<int,int> BorderSeed;

class volumeCalc
{
public:
	volumeCalc():_objMesh(NULL){}
	void       set_mesh(TriMesh* mesh);
	void       detect_borderline();
	//choose one borderline and it's corresponding seed
	void	   choose_border_seed(int b, int s);
	void	   start_compute();
	void	   draw();
	bool       set_highlight(int k);
	int	       get_highlight(vector<int>& hl);
	void	   selected_sub_mesh_area();
	void	   remove_a_sub_mesh_area();

	//it will be modified or deleted later.  2014/10/14
	void	   get_standard_curve(const TriMesh *mesh);

	~volumeCalc();
private:
	TriMesh*            _objMesh;
	vector<TriMesh*>    _subMesh;
	FeatureEdges	    _edgeDetector;
	vector<BorderSeed>  _bsIndex;
	vector<float>       _subVolume;
	stack<vector<int> > _subMeshFacets;
	float	   mesh_volume_calc(TriMesh* mesh);
	//BFS and generate a mesh 
	TriMesh*   generate_sub_mesh(const vector<int>& border, int seed);
	//generate sub mesh according to facets in stack
	void	   generate_all_sub_mesh();
	void	   generate_single_sub_mesh(const vector<int>& meshFacetsIndex);
};

#endif