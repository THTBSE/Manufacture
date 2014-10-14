
#pragma once
#include "ViewSpace/BackGround.h"
#include "ViewSpace/ShowShader.h"
#include "trimesh/include/TriMesh.h"
#include "VolumeCalc/VolumeCalc.h"
#include "Contoller/Controller.h"
using namespace ViewSpace;


class CGeometryProcDoc : public CDocument
{
protected: 
	CGeometryProcDoc();
	DECLARE_DYNCREATE(CGeometryProcDoc)

//View Function 
public:
	BackGroundColor	 m_BackGroundColor;
	ShaderModel		 m_ShaderModel;

//Mesh data
	vector<TriMesh*> meshes;
	int				 Current_mesh;

//Process Type Controller
	vfclController	 procController; 

//Volume Calculator
	volumeCalc		VCalcInstance;
public:


public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

public:
	virtual ~CGeometryProcDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	DECLARE_MESSAGE_MAP()


};
