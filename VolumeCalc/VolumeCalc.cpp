#include "VolumeCalc.h"
#include "../trimesh/include/TriMesh_algo.h"

volumeCalc::~volumeCalc()
{
	for_each(_subMesh.begin(),_subMesh.end(),[](TriMesh* mesh){delete mesh;
	mesh = NULL;});
}

void volumeCalc::set_mesh(TriMesh* mesh)
{
	assert(mesh);
	_objMesh = mesh;
}

float volumeCalc::mesh_volume_calc(TriMesh* mesh)
{
	assert(mesh);
	mesh_hole_fill(mesh);
	return mesh_volume(mesh);
}

void volumeCalc::selected_sub_mesh_area()
{
	if (_bsIndex.empty())
		return;
	const vector<int>& border = _edgeDetector.output_borderline(_bsIndex.back().first);
	const int seed = _bsIndex.back().second;
	_bsIndex.pop_back();
	_objMesh->need_neighbors();
	_objMesh->need_adjacentfaces();
	vector<int> selectedFacets;
	set<int> borderPoint(border.begin(), border.end());
	set<int> traversed;
	queue<int> bfsPoints;
	bfsPoints.push(seed);
	traversed.insert(seed);

	while (!bfsPoints.empty())
	{
		int pt = bfsPoints.front();
		bfsPoints.pop();

		for_each(_objMesh->adjacentfaces[pt].begin(), _objMesh->adjacentfaces[pt].end(),
			[this,&selectedFacets](int i){
			if (!_objMesh->faces[i].beSelect)
			{
				_objMesh->faces[i].beSelect = true;
				selectedFacets.push_back(i);
			}
				
		});
		for_each(_objMesh->neighbors[pt].begin(), _objMesh->neighbors[pt].end(),
			[&borderPoint, &traversed, &bfsPoints](int i){
			if (!borderPoint.count(i) && !traversed.count(i))
			{
				traversed.insert(i);
				bfsPoints.push(i);
			}
		});
	}

	if (!selectedFacets.empty())
		_subMeshFacets.push(selectedFacets);
}

void volumeCalc::remove_a_sub_mesh_area()
{
	if (_subMeshFacets.empty())
		return;
	vector<int>& subMesh = _subMeshFacets.top();
	std::for_each(subMesh.begin(), subMesh.end(), [this](int i)
	{_objMesh->faces[i].beSelect = false; });
	_subMeshFacets.pop();
}

void volumeCalc::generate_all_sub_mesh()
{
	if (_subMeshFacets.empty())
		return;
	while (!_subMeshFacets.empty())
	{
		generate_single_sub_mesh(_subMeshFacets.top());
		_subMeshFacets.pop();
	}
}

void volumeCalc::generate_single_sub_mesh(const vector<int>& meshFacetsIndex)
{
	TriMesh* mesh = new TriMesh;
	map<int, int> vertexMap;
	map<int, int>::iterator vIter;
	vector<TriMesh::Face>& trif = _objMesh->faces;
	std::for_each(meshFacetsIndex.begin(), meshFacetsIndex.end(), 
		[this, &vertexMap, &vIter,&trif,&mesh](int fIndex)
	{
		TriMesh::Face tmpf;
		for (int i = 0, j = 2; i<3; i++, j--)
		{
			vIter = vertexMap.find(trif[fIndex][i]);
			if (vIter == vertexMap.end())
			{
				mesh->vertices.push_back(_objMesh->vertices[trif[fIndex][i]]);
				tmpf[j] = mesh->vertices.size() - 1;
				vertexMap.insert(make_pair(trif[fIndex][i], tmpf[j]));
			}
			else
			{
				tmpf[j] = vIter->second;
			}
		}
		mesh->faces.push_back(tmpf);
	});

	_subMesh.push_back(mesh);
}

TriMesh* volumeCalc::generate_sub_mesh(const vector<int>& border, int seed)
{
	TriMesh* mesh = NULL;
	if (seed == -1)
		return mesh;
	
	mesh = new TriMesh;
	_objMesh->need_neighbors();
	_objMesh->need_adjacentfaces();
	set<int> borderPoint(border.begin(),border.end());
	set<int> traversed; 
	queue<int> bfsPoints;
	bfsPoints.push(seed);
	traversed.insert(seed);

	while (!bfsPoints.empty())
	{
		int pt = bfsPoints.front();
		bfsPoints.pop();

		for_each(_objMesh->adjacentfaces[pt].begin(),_objMesh->adjacentfaces[pt].end(),
			[this](int i){_objMesh->faces[i].beSelect = true;});
		for_each(_objMesh->neighbors[pt].begin(),_objMesh->neighbors[pt].end(),
			[&borderPoint,&traversed,&bfsPoints](int i){
				if (!borderPoint.count(i) && !traversed.count(i))
				{
					traversed.insert(i);
					bfsPoints.push(i);
				}
		});
	}

	map<int,int> vertexMap;
	map<int,int>::iterator vIter;
	for_each(_objMesh->faces.begin(),_objMesh->faces.end(),
		[this,&mesh,&vertexMap,&vIter](TriMesh::Face& trif){
			if (trif.beSelect)
			{
				TriMesh::Face tmpf;
				for (int i=0,j=2; i<3; i++,j--)
				{
					vIter = vertexMap.find(trif[i]);
					if (vIter == vertexMap.end())
					{
						mesh->vertices.push_back(_objMesh->vertices[trif[i]]);
						tmpf[j] = mesh->vertices.size() - 1;
						vertexMap.insert(make_pair(trif[i],tmpf[j]));
					}
					else
					{
						tmpf[j] = vIter->second;
					}
				}
				mesh->faces.push_back(tmpf);
			}
	});

	mesh->need_normals();
	return mesh;
}


void volumeCalc::detect_borderline()
{
	assert(_objMesh);
	if (!_objMesh)
		return;
	_edgeDetector.set_mesh(_objMesh);
	_edgeDetector.extract_feature_edges();
}

// bug was fixed ! nice ! 
void volumeCalc::start_compute()
{
	/*
	if (_bsIndex.empty())
	return;

	TriMesh* mesh = NULL;
	for_each(_bsIndex.begin(),_bsIndex.end(),[this,&mesh](BorderSeed bs){
	mesh = generate_sub_mesh(_edgeDetector.output_borderline(bs.first),bs.second);
	_subMesh.push_back(mesh);
	_subVolume.push_back(mesh_volume_calc(mesh));
	});
	*/

	if (_subMesh.empty())
		return;
	std::for_each(_subMesh.begin(), _subMesh.end(), [this](TriMesh* mesh)
	{
		_subVolume.push_back(mesh_volume_calc(mesh));
	});
}

void volumeCalc::choose_border_seed(int b, int s)
{
	_bsIndex.push_back(BorderSeed(b,s));
}

void volumeCalc::draw()
{
	_edgeDetector.draw_borderline();
	_edgeDetector.draw_project_curve();
}

bool volumeCalc::set_highlight(int k)
{
	return _edgeDetector.set_highlight(k);
}

int volumeCalc::get_highlight(vector<int>& hl)
{
	return _edgeDetector.get_highlight(hl);
}

void volumeCalc::get_standard_curve(const TriMesh* mesh)
{
	_edgeDetector.get_standard_curve(mesh);
}