#ifndef  CBackGround_H_
#define  CBackGround_H_
#pragma once
#include "../trimesh/include/TriMesh.h"
namespace ViewSpace
{
	enum BackGroundColor
	{
		CHANGE_GRAY,//灰色渐变
		CHANGE_DEONGAREE,//深蓝渐变
		CHANGE_LIGHT_BLUE//浅蓝渐变

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
