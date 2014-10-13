#include "Delaunay.h"
using namespace BaseClass;
CDelaunay::CDelaunay(vector<vec2>coudpoint)
{
	m_CloudPoint=coudpoint;
	m_NumberOfV=m_CloudPoint.size();
}
CDelaunay::CDelaunay(vector<vec2>coudpoint,vector<int>OuterRingID,vector<int>InnerRingID)
{
	m_CloudPoint=coudpoint;
	m_NumberOfV=m_CloudPoint.size();
	int ORN=OuterRingID.size();
	m_Edges.resize(ORN);
	for(int i=0;i<ORN;i++)
	{
		m_Edges[i].Start=OuterRingID[i];
		m_Edges[i].End=OuterRingID[(i+1)%ORN];
	}


}

CDelaunay::~CDelaunay(void)
{
}
int CDelaunay::FindXMin(vector<vec2>cloudpoint)
{   

	int xmin=0;
	for(int i=1;i<cloudpoint.size();i++)
	{
		if (cloudpoint[i][0]<cloudpoint[xmin][0])
		{
			xmin=i;
		}
	}
return xmin;
}
//��ʱ��˳��洢����Χ�Ķ�����ɵı�,����m_Edges��
void CDelaunay::FindBoundary(vector<vec2>cloudpoint)
{
	int startIndex=FindXMin(cloudpoint);
	int lastIndex=startIndex-1;
	int tempIndex=startIndex;
	Edge edge0;
	edge0.Start=startIndex;
	vec2 vector1(0,100);
	vec2  vector2;
	double vector1Length,vector2Length,angleTemp,angleMax,lengthMin;
	angleMax = 0;
	while (lastIndex != startIndex)
	{
		vector1Length = len(vector1);
		lengthMin = 300;
		for (int i= 0;i < cloudpoint.size();i++)//�ұ߽�
		{

			if(i!=edge0.Start)
			{
				vector2 =cloudpoint[i]-cloudpoint[tempIndex];
				vector2Length =len(vector2);
				angleTemp=acos((vector1 DOT vector2)/ (vector1Length * vector2Length));
				if (angleTemp > angleMax)
				{
					angleMax = angleTemp;
					edge0.End = i;
					lengthMin = vector2Length;
				}

				else if (angleTemp == angleMax && vector2Length < lengthMin)//���㹲�ߵ������ѡ�����start����ĵ�

				{
					edge0.End = i;
					lengthMin = vector2Length;
				}

			}

		}

		m_Edges.push_back(edge0);
		lastIndex=edge0.End;               
		edge0.Start = lastIndex;
		angleMax = 0;
		vector1 =cloudpoint[tempIndex]-cloudpoint[lastIndex];
		tempIndex = lastIndex;               
	}//while
	
}
void CDelaunay::GenerateTIN(vector<vec2>cloudpoint)
{
	//����TIN
	int newIndex;
	int edgeCount =m_Edges.size();
	double angleV1V2Temp, angleV2V3Temp, angleV1V2Max, angleV2V3Max, vector3Length,vector1Length,vector2Length;
	vec2 vector3,vector2,vector1;
	bool isTriExist;
	for (int i = 0; i < m_Edges.size(); i++)
	{
		Edge edge=m_Edges[i];//ȡ��һ����
		//�ж��������Ƿ���ڣ������ߵ��������Ѵ��ڣ�����������ǣ���
		if (edge.LeftTri == -1)
		{
			newIndex = -1;//��ѡȡ�������
			angleV1V2Max =0; angleV2V3Max =0;
			vector1 =cloudpoint[edge.End]-cloudpoint[edge.Start];
			for (int j = 0; j < cloudpoint.size(); j++)
			{
				if (j != edge.Start && j != edge.End)//�ų��ߵĶ˵�
				{
					vector2 =cloudpoint[j]-cloudpoint[edge.Start];
					if(vector1[0] * vector2[1] - vector1[1] * vector2[0]> 0)//�����j������vector1�����
					{
						vector3 =cloudpoint[j]-cloudpoint[edge.End];
						vector1Length=len(vector1);
						vector2Length=len(vector2);
						vector3Length=len(vector3);
						angleV1V2Temp =acos((vector1 DOT vector2)/(vector1Length * vector2Length));
						angleV2V3Temp =acos((vector2 DOT vector3)/ (vector2Length * vector3Length));

						if (angleV2V3Temp > angleV2V3Max)
						{

							angleV2V3Max = angleV2V3Temp;
							angleV1V2Max = angleV1V2Temp;
							newIndex = j;

						}
						else if (angleV2V3Temp == angleV2V3Max && angleV1V2Max >= angleV1V2Temp)
						{
							angleV1V2Max = angleV1V2Temp;
							newIndex = j;
						}

					}

				}

			} //for                  

			if (newIndex != -1)//���ҵ�����ôһ������Ҫ��ĵ�ͼ�¼������
			{                       
				Tri tri;
				tri.Node[0] = edge.Start;
				tri.Node[1] = edge.End;
				tri.Node[2] = newIndex;
				edge.LeftTri =m_Faces.size();//���ñߵ��������������
				isTriExist = false;
				//��¼��1
				for(int k = 0; k <m_Edges.size(); k++)
				{
					Edge tempEdge =m_Edges[k];
					if (tempEdge.Start == edge.Start && tempEdge.End == newIndex)
					{

						tempEdge.RightTri = m_Faces.size();
						tri.AdjTriB = tempEdge.LeftTri;
						isTriExist = true;
						break;

					}
					else if (tempEdge.Start == newIndex && tempEdge.End == edge.Start)
					{

						tempEdge.LeftTri = m_Faces.size();
						tri.AdjTriB = tempEdge.RightTri;
						isTriExist = true;
						break;
					}

				}

				if (isTriExist ==false)//�������������߾��½�һ����
				{
					Edge newEdge;
					newEdge.Start = newIndex;
					newEdge.End = edge.Start;
					newEdge.LeftTri = m_Faces.size();
					m_Edges.push_back(newEdge);
				}
				isTriExist = false;
				//��¼��2
				for (int k = 0; k <m_Edges.size(); k++)
				{
					Edge tempEdge = m_Edges[k];
					if (tempEdge.Start == newIndex && tempEdge.End == edge.End)
					{
						tempEdge.RightTri =m_Faces.size();
						tri.AdjTriA = tempEdge.LeftTri;
						isTriExist = true;
						break;
					}
					else if (tempEdge.Start == edge.End && tempEdge.End == newIndex)
					{
						tempEdge.LeftTri = m_Faces.size();
						tri.AdjTriA = tempEdge.RightTri;
						isTriExist = true;
						break;
					}
				}

				if (isTriExist == false)//�������������߾��½�һ����
				{
					Edge newEdge;
					newEdge.Start = edge.End ;
					newEdge.End = newIndex;
					newEdge.LeftTri =m_Faces.size();
					m_Edges.push_back(newEdge);

				}
				tri.AdjTriC = edge.RightTri;//���edge���������β����ڣ���������Ҳ������
				m_Faces.push_back(tri);
			}
		}

		else if (edge.RightTri == -1)
		{
			newIndex = -1;//��ѡȡ�������
			angleV1V2Max = 0; angleV2V3Max = 0;
			vector1=cloudpoint[edge.End]-cloudpoint[edge.Start];
			for(int j = 0; j < cloudpoint.size(); j++)
			{
				if(j != edge.Start && j != edge.End)//�ų��ߵĶ˵�
				{
					vector2=cloudpoint[j]-cloudpoint[edge.Start];
					if(vector1[0] * vector2[1] - vector1[1] * vector2[0] < 0)
					{
						vector3=cloudpoint[j]-cloudpoint[edge.End];
						vector1Length =len(vector1);
						vector2Length =len(vector2);
						vector3Length =len(vector3);
						angleV1V2Temp = acos((vector1 DOT vector2)/(vector1Length * vector2Length));
						angleV2V3Temp = acos((vector2 DOT vector3)/(vector2Length * vector3Length));

						if (angleV2V3Temp > angleV2V3Max)
						{
							angleV1V2Max = angleV1V2Temp;
							angleV2V3Max = angleV2V3Temp;
							newIndex = j;
						}
						else if (angleV2V3Temp == angleV2V3Max && angleV1V2Max <= angleV1V2Temp)
						{
							angleV1V2Max = angleV1V2Temp;
							newIndex = j;
						}

					}

				}

			}

			if (newIndex != -1)//���ҵ�����ôһ������Ҫ��ĵ�ͼ�¼������
			{
				Tri tri;
				tri.Node[0] = edge.Start;
				tri.Node[1]= edge.End;
				tri.Node[2]= newIndex;
				edge.RightTri =m_Faces.size();//���ñߵ��������������
				isTriExist = false;
				//��¼��1
				for (int k = 0; k < m_Edges.size(); k++)
				{
					Edge tempEdge = m_Edges[k];
					if (tempEdge.Start == newIndex && tempEdge.End ==edge.Start )
					{
						tempEdge.RightTri =m_Faces.size();
						tri.AdjTriB = tempEdge.LeftTri;
						isTriExist = true;
						break;
					}

					else if (tempEdge.Start == edge.Start && tempEdge.End == newIndex)
					{
						tempEdge.LeftTri =m_Faces.size();
						tri.AdjTriB = tempEdge.RightTri;
						isTriExist = true;
						break;
					}
				}

				if (isTriExist == false)//�������������߾��½�һ����
				{
					Edge newEdge;
					newEdge.Start = edge.Start;
					newEdge.End = newIndex;
					newEdge.LeftTri = m_Faces.size();
					m_Edges.push_back(newEdge);
				}

				isTriExist = false;
				//��¼��2
				for (int k = 0; k <m_Edges.size(); k++)
				{
					Edge tempEdge = m_Edges[k];
					if (tempEdge.Start == edge.End && tempEdge.End == newIndex)
					{
						tempEdge.RightTri =m_Faces.size();
						tri.AdjTriA = tempEdge.LeftTri;
						isTriExist = true;
						break;
					}

					else if (tempEdge.Start == newIndex && tempEdge.End == edge.End)
					{
						tempEdge.LeftTri =m_Faces.size();
						tri.AdjTriA = tempEdge.RightTri;
						isTriExist = true;
						break;
					}
				}

				if (isTriExist == false)//�������������߾��½�һ����
				{
					Edge newEdge ;
					newEdge.Start = newIndex;
					newEdge.End = edge.End;
					newEdge.LeftTri =m_Faces.size();
					m_Edges.push_back(newEdge);
				}
				tri.AdjTriC = edge.LeftTri;//���edge���������β����ڣ���������Ҳ������
				m_Faces.push_back(tri);
			}
		}
	}

}
vector<CDelaunay::Tri> CDelaunay::ComputeResult()
{
	FindBoundary(m_CloudPoint);
	GenerateTIN(m_CloudPoint);
	//�޸�������Ƭ���ص������
	int fn=m_Faces.size();
	vector<Tri>faces;
	for (vector<Tri>::iterator it=m_Faces.begin();it!=m_Faces.end();it++)
	{
		vector<Tri>::iterator it0=find(faces.begin(),faces.end(),*it);
		if(it0==faces.end()) faces.push_back(*it);
	}
	
    m_Faces=faces;
return m_Faces;
}

