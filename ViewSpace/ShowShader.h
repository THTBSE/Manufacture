#ifndef  CShowShader_H_
#define  CShowShader_H_
#pragma once
#include "..\TriMesh\include\TriMesh.h"

namespace ViewSpace
{
	enum ShaderModel
	{
		SMOOTH_SHADER,//平滑着色
		PLANE_SHADER,//平面着色
		WIRE_FRAME,//线框模式
		HIDE_WIRE_FRAME//隐藏线框

	}; 

class CShowShader
{
public:
	static void ShaderWireFrame(TriMesh*Tmesh);
	static void PlaneShaderModel(TriMesh*Tmesh);
	static void SmoothShaderModel(TriMesh*Tmesh);
	static void ShaderMesh(TriMesh*Tmesh,ShaderModel shadermodle0=SMOOTH_SHADER);
    static void HideWireFrame(TriMesh*Tmesh);








};

}
#endif
