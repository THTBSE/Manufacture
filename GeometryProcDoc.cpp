

#include "stdafx.h"

#ifndef SHARED_HANDLERS
#include "GeometryProc.h"
#endif

#include "GeometryProcDoc.h"

#include <propkey.h>
#include <chrono>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGeometryProcDoc

IMPLEMENT_DYNCREATE(CGeometryProcDoc, CDocument)

BEGIN_MESSAGE_MAP(CGeometryProcDoc, CDocument)
END_MESSAGE_MAP()


CGeometryProcDoc::CGeometryProcDoc()
{
	//View Function
	m_BackGroundColor = CHANGE_LIGHT_BLUE;
	m_ShaderModel = PLANE_SHADER;
}

CGeometryProcDoc::~CGeometryProcDoc()
{
}

BOOL CGeometryProcDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CGeometryProcDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString fn = lpszPathName;
	char* filename = NULL;
	auto UnicodeToAnsi = [](char** file, CString& fn)
	{
		int P_Length = fn.GetLength();
		int nBytes = WideCharToMultiByte(CP_ACP, 0, fn, P_Length, NULL, 0, NULL, NULL);
		*file = new char[nBytes + 1];
		memset(*file, 0, P_Length + 1);
		WideCharToMultiByte(CP_OEMCP, 0, fn, P_Length, *file, nBytes, NULL, NULL);
	};
	UnicodeToAnsi(&filename, fn);
	string filecheck(filename);
	filecheck = filecheck.substr(filecheck.size()-4);
	std::transform(filecheck.begin(), filecheck.end(), filecheck.begin(), tolower);

	if (filecheck == (".stl"))
	{
		TriMesh* mesh = NULL;
		mesh = TriMesh::read_stl_helper(filename);
		mesh->need_bbox();
		meshes.push_back(mesh);
		Current_mesh = meshes.size() - 1;
		return TRUE;
	}
	return FALSE;
}




void CGeometryProcDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}




#ifdef _DEBUG
void CGeometryProcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGeometryProcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


