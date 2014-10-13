#pragma once
#include "..\TriMesh\include\TriMesh.h"
namespace BaseClass{
class CVertexOnMesh
{
public:
	CVertexOnMesh(void){face_id=-1;v_id=-1;};
	~CVertexOnMesh(void){};
	


	vec vp;
	int v_id;//选中的顶点是网格顶点时
	int face_id;//选中的顶点时面点时
	double uvw[3];
};
}
