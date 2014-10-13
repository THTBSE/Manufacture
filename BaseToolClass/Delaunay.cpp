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
//逆时针顺序存储最外围的顶点组成的边,存于m_Edges中
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
		for (int i= 0;i < cloudpoint.size();i++)//找边界
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

				else if (angleTemp == angleMax && vector2Length < lengthMin)//三点共线的情况，选择距离start最近的点

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
	//生成TIN
	int newIndex;
	int edgeCount =m_Edges.size();
	double angleV1V2Temp, angleV2V3Temp, angleV1V2Max, angleV2V3Max, vector3Length,vector1Length,vector2Length;
	vec2 vector3,vector2,vector1;
	bool isTriExist;
	for (int i = 0; i < m_Edges.size(); i++)
	{
		Edge edge=m_Edges[i];//取出一条边
		//判断三角形是否存在（若本边的左三角已存在，则计算右三角）？
		if (edge.LeftTri == -1)
		{
			newIndex = -1;//新选取点的索引
			angleV1V2Max =0; angleV2V3Max =0;
			vector1 =cloudpoint[edge.End]-cloudpoint[edge.Start];
			for (int j = 0; j < cloudpoint.size(); j++)
			{
				if (j != edge.Start && j != edge.End)//排除边的端点
				{
					vector2 =cloudpoint[j]-cloudpoint[edge.Start];
					if(vector1[0] * vector2[1] - vector1[1] * vector2[0]> 0)//如果点j在向量vector1的左侧
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

			if (newIndex != -1)//若找到了这么一个满足要求的点就记录三角形
			{                       
				Tri tri;
				tri.Node[0] = edge.Start;
				tri.Node[1] = edge.End;
				tri.Node[2] = newIndex;
				edge.LeftTri =m_Faces.size();//设置边的左侧三角形索引
				isTriExist = false;
				//记录边1
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

				if (isTriExist ==false)//若不存在这条边就新建一条边
				{
					Edge newEdge;
					newEdge.Start = newIndex;
					newEdge.End = edge.Start;
					newEdge.LeftTri = m_Faces.size();
					m_Edges.push_back(newEdge);
				}
				isTriExist = false;
				//记录边2
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

				if (isTriExist == false)//若不存在这条边就新建一条边
				{
					Edge newEdge;
					newEdge.Start = edge.End ;
					newEdge.End = newIndex;
					newEdge.LeftTri =m_Faces.size();
					m_Edges.push_back(newEdge);

				}
				tri.AdjTriC = edge.RightTri;//如果edge的右三角形不存在，则左三角也不存在
				m_Faces.push_back(tri);
			}
		}

		else if (edge.RightTri == -1)
		{
			newIndex = -1;//新选取点的索引
			angleV1V2Max = 0; angleV2V3Max = 0;
			vector1=cloudpoint[edge.End]-cloudpoint[edge.Start];
			for(int j = 0; j < cloudpoint.size(); j++)
			{
				if(j != edge.Start && j != edge.End)//排除边的端点
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

			if (newIndex != -1)//若找到了这么一个满足要求的点就记录三角形
			{
				Tri tri;
				tri.Node[0] = edge.Start;
				tri.Node[1]= edge.End;
				tri.Node[2]= newIndex;
				edge.RightTri =m_Faces.size();//设置边的左侧三角形索引
				isTriExist = false;
				//记录边1
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

				if (isTriExist == false)//若不存在这条边就新建一条边
				{
					Edge newEdge;
					newEdge.Start = edge.Start;
					newEdge.End = newIndex;
					newEdge.LeftTri = m_Faces.size();
					m_Edges.push_back(newEdge);
				}

				isTriExist = false;
				//记录边2
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

				if (isTriExist == false)//若不存在这条边就新建一条边
				{
					Edge newEdge ;
					newEdge.Start = newIndex;
					newEdge.End = edge.End;
					newEdge.LeftTri =m_Faces.size();
					m_Edges.push_back(newEdge);
				}
				tri.AdjTriC = edge.LeftTri;//如果edge的右三角形不存在，则左三角也不存在
				m_Faces.push_back(tri);
			}
		}
	}

}
vector<CDelaunay::Tri> CDelaunay::ComputeResult()
{
	FindBoundary(m_CloudPoint);
	GenerateTIN(m_CloudPoint);
	//修复三角面片有重叠的情况
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

