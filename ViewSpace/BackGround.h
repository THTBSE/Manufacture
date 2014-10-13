#ifndef  CBackGround_H_
#define  CBackGround_H_
#pragma once
#include "../trimesh/include/TriMesh.h"
namespace ViewSpace
{
	enum BackGroundColor
	{
		CHANGE_GRAY,//��ɫ����
		CHANGE_DEONGAREE,//��������
		CHANGE_LIGHT_BLUE//ǳ������

	};  
class CBackGround
{
public:

	static void RenderBackGround(BackGroundColor backgroundcolor=CHANGE_LIGHT_BLUE);
	static void RenderInformation(TriMesh*Tmesh);
	static void	DrawText(const char* str, int flag=18);

};

}
#endif
