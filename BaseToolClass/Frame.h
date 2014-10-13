#ifndef  CFrame_H_
#define  CFrame_H_
#pragma once
#include "..\TriMesh\include\TriMesh.h"
#include "..\TriMesh\include\Vec.h"

using namespace std;
namespace BaseClass{
class CFrame
{
public:
	CFrame(void);
	CFrame(vec e10,vec e20,vec n0);
	~CFrame(void);
	vec e1;
	vec e2;
	vec n;
	vec &operator[] (int i) 
	{ 
			if(i==0)return e1;
			else if (i==1)return e2;
			else if (i==2)return n;
			else assert(0);	
	}
	static void GetFrame(vec vnormal,CFrame&result_frame,bool normalizeframe=true);
	static void SetFrame(vec vnormal,vec v0_vneighbor,CFrame &result_frame);
	static void Encode_To_Frame(CFrame localframe,CFrame origin_Frame,CFrame &target_Frame);
	static void Encode_To_Frame(CFrame localframe,vec global_vector,vec& local_vector);
	static void Decode_From_Frame(CFrame localframe,vec local_vector,vec &global_vector);
	static void Decode_From_Frame(CFrame localframe,CFrame relative_Frame,CFrame &decode_Frame);
	static vec RotateVector(vec planenormal,vec v1v2);
	static vec ProjectVector(vec planenormal,vec v1v2);


};
}
#endif
