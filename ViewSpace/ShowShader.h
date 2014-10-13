#ifndef  CShowShader_H_
#define  CShowShader_H_
#pragma once
#include "..\TriMesh\include\TriMesh.h"

namespace ViewSpace
{
	enum ShaderModel
	{
		SMOOTH_SHADER,//ƽ����ɫ
		PLANE_SHADER,//ƽ����ɫ
		WIRE_FRAME,//�߿�ģʽ
		HIDE_WIRE_FRAME//�����߿�

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
