#pragma once
#include "..\TriMesh\include\TriMesh.h"
namespace BaseClass{
class CVertexOnMesh
{
public:
	CVertexOnMesh(void){face_id=-1;v_id=-1;};
	~CVertexOnMesh(void){};
	


	vec vp;
	int v_id;//ѡ�еĶ��������񶥵�ʱ
	int face_id;//ѡ�еĶ���ʱ���ʱ
	double uvw[3];
};
}
