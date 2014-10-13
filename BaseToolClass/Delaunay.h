#pragma once
#include "../TriMesh/include/Vec.h"
#include <vector>
#include <set>
#include <algorithm>
#include<map>


using namespace std;
namespace BaseClass{
class CDelaunay
{
public:
	CDelaunay(vector<vec2>coudpoint);
	CDelaunay(vector<vec2>coudpoint,vector<int>OuterRingID,vector<int>InnerRingID);
	~CDelaunay(void);
    
	class Edge
	{
	  public: 
		  Edge(void){LeftTri=-1;RightTri=-1;}
		  ~Edge(void){};
		  int Start;//�ߵ����
          int End;//�ߵ��յ�
	      int LeftTri ;//�������������
		  int RightTri ;//�ұ�����������
	};

	class Tri
	{	
	    public: 
			Tri(void){AdjTriA=-1;AdjTriB=-1;AdjTriC=-1;}
			~Tri(void){};
			int Node[3];//�����ڵ������
	        int AdjTriA ;//��һ���ڽ�����������
		    int AdjTriB ;//�ڶ����ڽ�����������
		    int AdjTriC;//�������ڽ�����������
			bool operator==(Tri const f2)const
			{   
				vector<int>f1v(3);
				vector<int>f2v(3);
				for (int i=0;i<3;i++)
				{
					f1v[i]=Node[i];
					f2v[i]=f2.Node[i];
				}
				sort(f1v.begin(),f1v.end());
				sort(f2v.begin(),f2v.end());
				for (int i=0;i<3;i++)
				{
					if(f1v[i]!=f2v[i])return false;
				}
				return true;
			}

	};
	
	void FindBoundary(vector<vec2>cloudpoint);
	int FindXMin(vector<vec2>cloudpoint);
	void GenerateTIN(vector<vec2>cloudpoint);



	vector<Tri> ComputeResult();
private:
	vector<Tri>m_Faces;
	vector<Edge>m_Edges;
	vector<vec2>m_CloudPoint;
	int m_NumberOfV;

};
}
