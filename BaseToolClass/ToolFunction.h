#pragma once


#include "../trimesh/include/TriMesh.h"
//
#include "VertexOnMesh.h"

namespace BaseClass{
class CToolFunction
{
public:
	static bool Extract_Beselect_Mesh(TriMesh * CTmesh,TriMesh*&ExtractMesh,vector<int>&Map_Extract_To_Base_V,vector<int>&Map_Extract_To_Base_F,bool is_optimise=false);
	static void FindBdy_3Ring(TriMesh *Tmesh);
	static vector<int> FindBorderV(TriMesh*Tmesh);
	static void FindBorderV(TriMesh*Tmesh,int BoundaryFlag,vector<int>&BorderVertex);

	static bool PointonMesh(vec vpoint,TriMesh*Tmesh,CVertexOnMesh &SelectV);
    static bool NearestPointMeshV(vec vi,TriMesh*Tmesh,int &vIndex);
    static bool FindNearestFace(TriMesh*Tmesh,point vi,int &Findex);
	//static void ScreenToPoint(CPoint P,COpenGLDC* pGLDC, vec &wp);

};
}
