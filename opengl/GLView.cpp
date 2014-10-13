// VRView.cpp : implementation of the CGLView class
//

#include "OpenGLDC.h"
//#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLView

IMPLEMENT_DYNCREATE(CGLView, CView)

BEGIN_MESSAGE_MAP(CGLView, CView)
	//{{AFX_MSG_MAP(CGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLView construction/destruction

CGLView::CGLView()
{
	m_pGLDC = NULL;
}

CGLView::~CGLView()
{
	if (m_pGLDC)
	{
		m_pGLDC=NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CGLView drawing

void CGLView::OnDraw(CDC* pDC)
{

	if(m_pGLDC){
		m_pGLDC->Ready();
		RenderScene(m_pGLDC);
		m_pGLDC->m_Camera.projection2D(); //2010.5.23��ӣ��ڻ�����ά������תΪ��άͶӰ
		DrawLegend(m_pGLDC);                //�Ա�������Ļ�Ĺ̶�����ͼ����˵�����ֵ�����
		m_pGLDC->Finish();
	}



}

void CGLView::DrawLegend(COpenGLDC* pDC)
{

}
BOOL CGLView::PreCreateWindow(CREATESTRUCT& cs)
{

	//  Add Window style required for OpenGL before window is created
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGLView message handlers
void CGLView::RenderScene(COpenGLDC* pDC)
{
	//pDC->DrawCoord();
}

int CGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pGLDC = new COpenGLDC(this->GetSafeHwnd());
	m_pGLDC->InitDC();
	return 0;
}

void CGLView::OnDestroy() 
{
	if(m_pGLDC)	delete m_pGLDC;	
	CView::OnDestroy();
}

void CGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if (cy > 0)
	{
		if(m_pGLDC)	
			m_pGLDC->GLResize(cx,cy);	
	}
	ZoomAll(); 
}

BOOL CGLView::OnEraseBkgnd(CDC* pDC) 
{
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CGLView::OnViewType(UINT type)
{
	ASSERT(type >= VIEW_FRONT && type <= VIEW_NW_ISOMETRIC);
	m_pGLDC->m_Camera.set_view_type(type);
	Invalidate();
}

void CGLView::Orient_To_Plane(CVector3D nor)
{
	m_pGLDC->m_Camera.View_To_Plane(nor);
	Invalidate();
}

void CGLView::OnInitialUpdate()
{
	ZoomAll();
}
BOOL CGLView::GetBox(double& x0,double& y0,double& z0,double& x1,double& y1,double& z1)
{
	return FALSE;
}

void CGLView::ZoomAll()
{
	double x0,y0,z0,x1,y1,z1;
	if(GetBox(x0,y0,z0,x1,y1,z1)){
		m_pGLDC->m_Camera.zoom_all(x0,y0,z0,x1,y1,z1);
		Invalidate();
	}
}

void CGLView::Zoom(double dScale)
{
	m_pGLDC->m_Camera.zoom(dScale);
	Invalidate();
}

void CGLView::MoveView(double dpx,double dpy)
{
	m_pGLDC->m_Camera.move_view(dpx,dpy);
	Invalidate();
}
void CGLView::RotateView(double dRotX,double dRotY,double dRotZ)
{
	m_pGLDC->RotateView(dRotX, dRotY, dRotZ);
	Invalidate();
	
}

//�Ƹ���ʸ����ת
void CGLView::RotateView(double dRot,VECTOR3D vect)
{
	m_pGLDC->RotateView(dRot, vect);
	Invalidate();
}
//����Ļ��������ת
void CGLView::RotateView(double dRot,short snXyz)
{
	if (GetViewName()=="TOP" || GetViewName()=="FRONT")
	{
		return;
	}
	m_pGLDC->RotateView(dRot, snXyz);
	Invalidate();
}

void CGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	int nMoveX, nMoveY;

	if (nFlags & MK_CONTROL)
	{
		if (nFlags & MK_RBUTTON)
		{
			nMoveX = point.x - m_pntRBntDown.x;
			nMoveY = m_pntRBntDown.y - point.y;
			m_pntRBntDown = point;
		}
		else
		{
			if (nFlags & MK_LBUTTON )
			{
				nMoveX = point.x - m_pntLBntDown.x;
				nMoveY = m_pntLBntDown.y - point.y;
				MoveView(nMoveX*0.0011,nMoveY*0.0011);
				m_pntLBntDown = point;
			}
			else
			{
				if (nFlags & MK_MBUTTON)
				{
					nMoveX = point.x - m_pntMBntDown.x;
					nMoveY = m_pntMBntDown.y - point.y;
					m_pntMBntDown = point;

					if (nMoveX > 3)
					{
						//RotateView(0, ROTATE_ANGLE, 0);

						RotateView(ROTATE_ANGLE, 1);
					}
					else
					{
						if(nMoveX < -3)
						{
							//RotateView(0, ROTATE_ANGLE_NEG, 0);
							RotateView(ROTATE_ANGLE_NEG, 1);
						}

					}

					if (nMoveY > 3)
					{
						//RotateView(ROTATE_ANGLE_NEG, 0, 0);
						RotateView(ROTATE_ANGLE_NEG, 0);
					} 
					else
					{
						if (nMoveY < -3)
						{
							//RotateView(ROTATE_ANGLE, 0, 0);
							RotateView(ROTATE_ANGLE, 0);
						}
					}
				}
			}
		}
	}

	Invalidate();

	CView::OnMouseMove(nFlags, point);
}

void CGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pntLBntDown = point;

	CView::OnLButtonDown(nFlags, point);
}

void CGLView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_pntMBntDown = point;

	CView::OnMButtonDown(nFlags, point);
}

void CGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_pntRBntDown = point;

	CView::OnRButtonDown(nFlags, point);
}

BOOL CGLView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	short old_zDelta = 0;
	if (nFlags & MK_CONTROL)
	{
		if(zDelta > old_zDelta)
			Zoom(1.1);
		if(zDelta < old_zDelta)
			Zoom(0.9);
		old_zDelta=zDelta;	
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
CString	CGLView::GetViewName()
{
	return m_strViewName;
}
void	CGLView::SetViewName(LPCTSTR str)
{
	m_strViewName = str;
}

//------------------------------------------------------------2010.8.22
// AUX_RGBImageRec * CGLView::LoadBMP(CHAR *Filename)
// {
// 	FILE *File=NULL;									// �ļ����
// 	if (!Filename)												// ȷ���ļ������ṩ
// 	{
// 		return NULL;										// ���û�ṩ������ NULL
// 	}
// 	File=fopen(Filename,"r");							// ���Դ��ļ�
// 	if (File)														// �ļ�����ô?
// 	{
// 		fclose(File);											// �رվ��
// 		return auxDIBImageLoadA(Filename);	// ����λͼ������ָ��
// 	}
// 	return NULL;											// �������ʧ�ܣ����� NULL
// }
// int CGLView::LoadGLTextures(CHAR *Filename)
// {
// 	int Status=FALSE;													// ״ָ̬ʾ��
// 	AUX_RGBImageRec *TextureImage[1];					// ��������Ĵ洢�ռ�
// 	memset(TextureImage,0,sizeof(void *)*1);					// ��ָ����Ϊ NULL
// 	// ����λͼ��������޴������λͼû�ҵ����˳�
// 	if (TextureImage[0]=LoadBMP(Filename))
// 	{
// 		Status=TRUE;														// �� Status ��Ϊ TRUE
// 		glGenTextures(1, &texture[0]);								// ��������
// 
// 		// ʹ������λͼ�������� �ĵ�������
// 		glBindTexture(GL_TEXTURE_2D, texture[0]);
// 		// ��������
// 		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
// 
// 		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// �����˲�
// 		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// �����˲�
// 	}
// 
// 	if (TextureImage[0])												// �����Ƿ����
// 	{
// 		if (TextureImage[0]->data)									// ����ͼ���Ƿ����
// 		{
// 			free(TextureImage[0]->data);							// �ͷ�����ͼ��ռ�õ��ڴ�
// 		}
// 
// 		free(TextureImage[0]);										// �ͷ�ͼ��ṹ
// 	}
// 	return Status;															// ���� Status
// }
// 


