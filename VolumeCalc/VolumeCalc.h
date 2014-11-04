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
#include "../FeatureEdges/baseExtract.h"
#include "../BaseToolClass/CGGToolkit.h"
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <memory>
using std::vector;
using std::set;
using std::queue;
using std::map;
using std::stack;

typedef std::pair<int,int> BorderSeed;

class volumeCalc
{
public:
	volumeCalc():_objMesh(NULL),planeType(0) {}
	void       set_mesh(TriMesh* mesh);
	//set the method for getting feature Edges 
	void	   set_edgeGetter(int method);
	void       detect_borderline();
	//choose one borderline and it's corresponding seed
	void	   choose_border_seed(int b, int s);
	void	   start_compute();
	void	   draw();
	//bool       set_highlight(int k);
	//int	       get_highlight(vector<int>& hl);
	void	   selected_sub_mesh_area();
	void	   remove_a_sub_mesh_area();

	//I need only a function to set all of the planes
	//It's more cool than use two functions ! ha !
	void	   set_plane(const Plane& plane);
	void	   set_plane_type(int i) { planeType = i; }
	~volumeCalc();
private:
	int planeType;
	std::shared_ptr<featureEdges> _edgeGetter;
	TriMesh*            _objMesh;
	vector<TriMesh*>    _subMesh;
	vector<BorderSeed>  _bsIndex;
	vector<float>       _subVolume;
	stack<vector<int> > _subMeshFacets;
	float	   mesh_volume_calc(TriMesh* mesh);
	//generate sub mesh according to facets in stack
	void	   generate_all_sub_mesh();
	void	   generate_single_sub_mesh(const vector<int>& meshFacetsIndex);
};

#endif