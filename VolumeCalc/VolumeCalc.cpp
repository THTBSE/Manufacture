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
	_edgeGetter->getTriInside(_subMeshFacets);
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


void volumeCalc::detect_borderline()
{
	assert(_objMesh);
	if (!_objMesh)
		return;
	_edgeGetter->getBaseEdges();
	_edgeGetter->getContour();
}

// bug was fixed ! nice ! 
void volumeCalc::start_compute()
{
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
	if (_edgeGetter)
		_edgeGetter->drawContour();
}

//bool volumeCalc::set_highlight(int k)
//{
//	return _edgeDetector.set_highlight(k);
//}
//
//int volumeCalc::get_highlight(vector<int>& hl)
//{
//	return _edgeDetector.get_highlight(hl);
//}

void volumeCalc::set_edgeGetter(int method)
{
	if (_edgeGetter)
		return;
	switch (method)
	{
	case 1:
	{
		_edgeGetter = std::make_shared<baseExtract>(baseExtract());
		_edgeGetter->setMesh(_objMesh);
		break;
	}
		default:
		break;
	}
}

void volumeCalc::set_plane(const Plane& plane)
{
	assert(_edgeGetter);
	switch (planeType)
	{
	case 0:
	{
		_edgeGetter->setBasePlane(plane);
		break;
	}
	case 1:
	{
		_edgeGetter->addConstraintPlane(plane, true);
		break;
	}
	case 2:
	{
		_edgeGetter->addConstraintPlane(plane, false);
		break;
	}
	default:
		break;
	}
}