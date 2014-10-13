/*************************************************************************
* This head file is for 3D scene script. We will define a scene class    *
* with some light and camera and models.                                 *
*************************************************************************/
#ifndef _CAMERA_H__
#define _CAMERA_H__

#include "cadbase.h"
#include "shitu.h"

class  GCamera
{

protected:
	//eye coordinator
    CPoint3D			m_eye;
	CPoint3D			m_ref;
	CVector3D		m_vecUp;

	//viewing volume
    double				m_far, m_near;
	double	 			m_width,m_height;//��������ϵ�µĲü����δ�С

	//viewport
	double				m_screen[2];

	CPoint3D			m_refCenter;

	GLdouble			ox, oy, oz;     //��ȡ��Ļ�����������
	GLint				CurrentViewport[4];				//��ǰ�ӿ�
	GLdouble			CurrentModelMatrix[16];		//��ǰģ�;���
	GLdouble			CurrentProjMatrix[16];			//��ǰͶӰ����


public:
    GCamera();
	~GCamera();

	//initailizing
	void init();

	GLdouble ModelMatrix[16], ProjMatrix[16];  //xin

	//�������Ӻ���
	void GetWxyz(double &x, double &y, double &z);      //�����Ļ�����������
	void Getjuzhen(GLint Viewport[4], GLdouble ModelMatrix[16], GLdouble ProjMatrix[16]); //��ȡ��ǰ����ؾ���
	void BeginGetMatrix();    //���о����ȡ׼������
	void GetCurrentView(CPoint3D &eye, CPoint3D &refp, CVector3D &vecUp);  //��õ�ǰ����ͼ���ӵ���Ϣ
	void SetBackView(CPoint3D eye, CPoint3D refp, CVector3D vecUp);//����ͼ���ر����״̬

    void projection();
	void selection(int xPos,int yPos);
	void endSel(int xPos,int yPos);

	void projection2D();//2010.5.23
	void ProjectionToScreen();

	//zooming
    void zoom(double scale); 
	void zoom_all(double x0,double y0,double z0,double x1,double y1,double z1);

	//switch into a classical view
    void set_view_type(int type);
	void View_To_Plane(CVector3D nor);

	//move view with precent
	void move_view(double dpx, double dpy);

	//set viewport acoording to window
	void set_screen( int x, int y);

	//set eye coordinate
	void set_eye(double eye_x,double eye_y,double eye_z);
	void set_ref(double ref_x,double ref_y,double ref_z);
	void set_vecUp(double up_dx,double up_dy,double up_dz);

	//set viewing volume
	void set_view_rect(double width,double height);
	void get_view_rect(double& width,double& height);

	//�����ӵ�����
	void center_view(double centerX,double centerY,double centerZ);

	//��ͼ��ת
	void rotate_x_view(double dx);
	void rotate_y_view(double dy);
	void rotate_z_view(double dz);
	void rotate_view(double dRot,VECTOR3D vect);
	void rotate_view(double dRot,short snXyz);

	void view_to_camera(double &x,double &y);

protected:
    void update_upVec();
};
#endif 

/////////////////