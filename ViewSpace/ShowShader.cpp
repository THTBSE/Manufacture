
#include "ShowShader.h"
#include "..\opengl\glew.h"
#include "..\BaseToolClass\OpenglToolFunction.h"
using namespace ViewSpace;
void CShowShader::ShaderWireFrame(TriMesh* Tmesh)
{      
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.0f,0.0f,1.0f);

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	int fn=Tmesh->faces.size();
	vector<unsigned int>FaceIndexArray_All(fn*3);
	for (int i=0;i<fn;i++)
	{
		FaceIndexArray_All[3*i]=Tmesh->faces[i][0];
		FaceIndexArray_All[3*i+1]=Tmesh->faces[i][1];
		FaceIndexArray_All[3*i+2]=Tmesh->faces[i][2];
	}
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glVertexPointer(3,GL_FLOAT,0,&Tmesh->vertices[0][0]);
	glDrawElements(GL_TRIANGLES,fn*3,GL_UNSIGNED_INT,&FaceIndexArray_All[0]);
	glDisable(GL_COLOR_MATERIAL);


}
void CShowShader::HideWireFrame(TriMesh*Tmesh)
{      
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.0f,0.0f,1.0f);
	glEnable(GL_CULL_FACE);	
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	int fn=Tmesh->faces.size();
	vector<unsigned int>FaceIndexArray_All(fn*3);
	for (int i=0;i<fn;i++)
	{
		FaceIndexArray_All[3*i]=Tmesh->faces[i][0];
		FaceIndexArray_All[3*i+1]=Tmesh->faces[i][1];
		FaceIndexArray_All[3*i+2]=Tmesh->faces[i][2];
	}
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glVertexPointer(3,GL_FLOAT,0,&Tmesh->vertices[0][0]);
	glDrawElements(GL_TRIANGLES,fn*3,GL_UNSIGNED_INT,&FaceIndexArray_All[0]);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	

}
//下面函数之所以这样复杂，是为了速度考虑，尽量避免调用opengl绘制函数可以大大减小绘制时间
//把选中与未被选中的模型分开画，这样可以减少调用材质设置函数
void CShowShader::SmoothShaderModel(TriMesh*Tmesh)
{
	Tmesh->need_normals();
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	int fn=Tmesh->faces.size();
	glDisable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glVertexPointer(3,GL_FLOAT,0,&Tmesh->vertices[0][0]);
	glNormalPointer(GL_FLOAT,0,&Tmesh->normals[0][0]);
	vector<int>noselectF;
	vector<int>beselectF;
	noselectF.reserve(fn);
	beselectF.reserve(fn);
	for (int i=0;i<fn;i++)
	{
		if(Tmesh->faces[i].beSelect) beselectF.push_back(i);
		else    noselectF.push_back(i);
	}

	int fn_be=beselectF.size();
	int fn_no=noselectF.size();
	vector<unsigned int> beselectF_Index(3*beselectF.size());
	vector<unsigned int> noselectF_Index(3*noselectF.size());
#pragma omp parallel for if(fn_be>1e3)
	for (int i=0;i<fn_be;i++)
	{   
		for (int j=0;j<3;j++)
		{
			beselectF_Index[3*i+j]=Tmesh->faces[beselectF[i]][j];
		}
	}
#pragma omp parallel for  
	for (int i=0;i<fn_no;i++)
	{   
		for (int j=0;j<3;j++)
		{
			noselectF_Index[3*i+j]=Tmesh->faces[noselectF[i]][j];
		}
	}
	if (!noselectF_Index.empty())glDrawElements(GL_TRIANGLES,fn_no*3,GL_UNSIGNED_INT,&noselectF_Index[0]);


	if (!beselectF_Index.empty())
	{
		COpenglToolFunction::SetMaterialColor(RGB(255,20,147));//粉红
		glDrawElements(GL_TRIANGLES,fn_be*3,GL_UNSIGNED_INT,&beselectF_Index[0]);
	}

}
//下面函数之所以这样复杂，是为了速度考虑，尽量避免调用opengl绘制函数可以大大减小绘制时间
//把选中与未被选中的模型分开画，这样可以减少调用材质设置函数
void CShowShader::PlaneShaderModel(TriMesh*Tmesh)
{
	Tmesh->need_Facenormals();
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	int fn=Tmesh->faces.size();
	for (int i=0;i<fn;i++)
	{
		if(Tmesh->faces[i].beSelect)continue;
		glBegin(GL_TRIANGLES);
		glNormal3f(Tmesh->FaceNormal[i][0],Tmesh->FaceNormal[i][1],Tmesh->FaceNormal[i][2]);
		TriMesh::Face &f=Tmesh->faces[i];
		vector<vec>&v=Tmesh->vertices;
		glVertex3f(v[f[0]][0],v[f[0]][1],v[f[0]][2]);
		glVertex3f(v[f[1]][0],v[f[1]][1],v[f[1]][2]);
		glVertex3f(v[f[2]][0],v[f[2]][1],v[f[2]][2]);
		glEnd();

	}

	bool flag=false;
	for (int i=0;i<fn;i++)
	{
		if(Tmesh->faces[i].beSelect)
		{
			COpenglToolFunction::SetMaterialColor(RGB(255,20,147));
			flag=true;
			break;
		}		
	}
	if(flag)
	for (int i=0;i<fn;i++)
	{
		if(Tmesh->faces[i].beSelect)
		{
			glBegin(GL_TRIANGLES);
			glNormal3f(Tmesh->FaceNormal[i][0],Tmesh->FaceNormal[i][1],Tmesh->FaceNormal[i][2]);
			TriMesh::Face &f=Tmesh->faces[i];
			vector<vec>&v=Tmesh->vertices;
			glVertex3f(v[f[0]][0],v[f[0]][1],v[f[0]][2]);
			glVertex3f(v[f[1]][0],v[f[1]][1],v[f[1]][2]);
			glVertex3f(v[f[2]][0],v[f[2]][1],v[f[2]][2]);
			glEnd();
		}
	}
	
}
void CShowShader::ShaderMesh(TriMesh*Tmesh,ShaderModel shadermodle0)
{
	Tmesh->need_bbox();
	switch(shadermodle0)
	{
	case SMOOTH_SHADER:
		{
			SmoothShaderModel(Tmesh);
			break;
		}
	case WIRE_FRAME:
		{
			ShaderWireFrame(Tmesh);
			break;
		}
	case PLANE_SHADER:
		{
			PlaneShaderModel(Tmesh);
			break;
		}
	case HIDE_WIRE_FRAME:
		{
			HideWireFrame(Tmesh);
			break;
		}
	default:SmoothShaderModel(Tmesh);
	}

}




