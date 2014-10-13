#include"..\stdafx.h"
#include "BackGround.h"
#include "..\opengl\glew.h"
#include "..\opengl\glut.h"
#include <sstream>
#include <string>
using namespace ViewSpace;

void CBackGround::RenderBackGround(BackGroundColor backgroundcolor)
{
	//��¡
	// ����������뵥λ����
	glEnable(GL_COLOR_MATERIAL);//���ö�����ɫ��û�����õ������glColor4f��������
	glDisable(GL_LIGHTING);////��Ϊ����ɫ��Ҫ��ʱ�رչ��գ�������������ɢɫ�ⷴ�䣬�Ӷ�Ӱ�챳������
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	switch(backgroundcolor)
	{
	case BackGroundColor::CHANGE_LIGHT_BLUE:
		{
			glColor4f(92.0/255,125.0/255,169.0/255,1);
			glVertex3f(-1, -1,0.99);	
			glColor4f(198.0/255,220.0/255,236.0/255,1);
			glVertex3f(-1,  1,0.99);	
			glColor4f(198.0/255,220.0/255,236.0/255,1);
			glVertex3f(1,  1,0.99);
			glColor4f(92.0/255,125.0/255,169.0/255,1);	
			glVertex3f( 1, -1,0.99);
			break;
		}
	case BackGroundColor::CHANGE_GRAY:
		{
			glColor4f(227.0/255,225.0/255,213.0/255,1);
			glVertex3f(-1, -1,0.99);	
			glColor4f(158.0/255,155.0/255,145.0/255,1);
			glVertex3f(-1,  1,0.99);	
			glColor4f(158.0/255,155.0/255,145.0/255,1);
			glVertex3f(1,  1,0.99);
			glColor4f(227.0/255,225.0/255,213.0/255,1);	
			glVertex3f( 1, -1,0.99);
			break;
		}
	case BackGroundColor::CHANGE_DEONGAREE:
		{
			glColor4f(0.0/255,93.0/255,155.0/255,1);
			glVertex3f(-1, -1,0.99);	
			glColor4f(0.0/255,17.0/255,50.0/255,1);
			glVertex3f(-1,  1,0.99);	
			glColor4f(0.0/255,17.0/255,50.0/255,1);
			glVertex3f(1,  1,0.99);
			glColor4f(0.0/255,93.0/255,155.0/255,1);
			glVertex3f( 1, -1,0.99);
			break;
		}
	default:
		{
			glColor4f(92.0/255,125.0/255,169.0/255,1);
			glVertex3f(-1, -1,0.99);	
			glColor4f(198.0/255,220.0/255,236.0/255,1);
			glVertex3f(-1,  1,0.99);	
			glColor4f(198.0/255,220.0/255,236.0/255,1);
			glVertex3f(1,  1,0.99);
			glColor4f(92.0/255,125.0/255,169.0/255,1);	
			glVertex3f( 1, -1,0.99);
			break;
		}

	}


	glEnd();

	// �ָ�ԭ���ľ���
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);//���ò����Լ�����ɫ�����Ƕ�����ɫ
	glEnable(GL_LIGHTING);//�򿪹���
}
//�������꼰ģ�Ͷ���������Ƭ��Ϣ
void CBackGround::RenderInformation(TriMesh* Tmesh)
{
	glEnable(GL_COLOR_MATERIAL);//���ö�����ɫ��û�����õ������glColor4f��������
	glDisable(GL_LIGHTING);////��Ϊ����ɫ��Ҫ��ʱ�رչ��գ�������������ɢɫ�ⷴ�䣬�Ӷ�Ӱ�챳������
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0,0,0);
	glRasterPos2f(-0.99,-0.97);
	DrawText("triangles:",12);
	std::stringstream ss,ss1;
	ss << Tmesh->faces.size();
	string str = ss.str();
	const char *pBuff;
	pBuff = str.c_str();
	DrawText(pBuff,12);
	glRasterPos2f(-0.99,-0.90);
	DrawText("vertices:",12);
	
	
	ss1 << Tmesh->vertices.size();
	str = ss1.str();
	pBuff = str.c_str();

	DrawText(pBuff,12);
	// �ָ�ԭ���ľ���
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);//���ò����Լ�����ɫ�����Ƕ�����ɫ
	glEnable(GL_LIGHTING);//�򿪹���
}
void CBackGround::DrawText(const char* str,int flag)
{
	const char* p = NULL;

	for (p = str; *p; p++)
	{
		if(flag==24)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
		else if(flag==18)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
		else
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
	}
}

