#ifndef _CIRCLESELECTION_H_
#define _CIRCLESELECTION_H_
#include "OpenglSelection.h"
#include "../trimesh/include/TriMesh.h"
#include "../opengl/OpenGLDC.h"

class circleSelection
{
public:
	static void toScreen(TriMesh* mesh, vector<int>& vIndex, vector<vec2>& sp
		,COpenGLDC* PGLRC);
	static vec2 getPointWithinCircle(vector<vec2>& sp);
};



#endif