#include "../stdafx.h "
#include "ToolFunction.h"
#include "../TriMesh\include\TriMesh_algo.h"
using namespace BaseClass;
bool CToolFunction::Extract_Beselect_Mesh(TriMesh * CTmesh,TriMesh*&ExtractMesh,vector<int>&Map_Extract_To_Base_V,vector<int>&Map_Extract_To_Base_F,bool is_optimise)
{   
	TriMesh*Tmesh=CTmesh;
	Tmesh->need_neighbors();
	Tmesh->need_adjacentedges();
	Tmesh->need_adjacentfaces();
	TriMesh*NewMesh =new TriMesh();
	int vn=Tmesh->vertices.size();
	int fn=Tmesh->faces.size();
	//处理内外角点
	vector<int>beselectF;
	beselectF.reserve(fn);
	
	if(is_optimise)
	{
	vector<bool>insertflagf(fn,false);
	for (int i=0;i<fn;i++)
	{
		if (Tmesh->faces[i].beSelect==true)
		{   
			for (int j=0;j<3;j++)
			{   
				int vid=Tmesh->faces[i][j];
				int afn=Tmesh->adjacentfaces[vid].size();
				vector<int>&af=Tmesh->adjacentfaces[vid];
				int countaf=0;//计算邻接三角面片中被选中的三角面片
				int noselectf;
				for (int k=0;k<afn;k++)
				{   
					TriMesh::Face f0=Tmesh->faces[af[k]];
					if (f0.beSelect==true)
					{
						countaf++;
					}
					else noselectf=af[k];
				}
				if (countaf==1)continue;//外角点
				if (insertflagf[i]==false)
				{
					beselectF.push_back(i);
					insertflagf[i]=true;
				}
				//内角点
				if(countaf==(afn-1))
				{        
					if (insertflagf[noselectf]==false)
					{
						beselectF.push_back(noselectf);
						insertflagf[noselectf]=true;
					}		
				}
			}
		}

	}//for
	}//if
	else 
		for (int i=0;i<fn;i++)
		{
			if (Tmesh->faces[i].beSelect==true)
			{ 
				beselectF.push_back(i);
			}
		}
	vector<bool>Visitflag(vn,false);
	vector<int>Vnewid(vn);
	TriMesh::Face newface;
	int fnbeselect=beselectF.size();
	if (fnbeselect==0)
	{
		AfxMessageBox("警告：未发现被选中的区域，或选中的区域过小");
		return false;
	}
	NewMesh->faces.resize(fnbeselect);
	NewMesh->vertices.reserve(vn);
	Map_Extract_To_Base_V.reserve(vn);
	Map_Extract_To_Base_F.resize(fnbeselect);
	for (int i=0;i<fnbeselect;i++)
	{
		for (int j=0;j<3;j++)
		{   
			int vid=Tmesh->faces[beselectF[i]][j];
			if (Visitflag[vid]==false)
			{   
				Vnewid[vid]=NewMesh->vertices.size();
				NewMesh->vertices.push_back(Tmesh->vertices[vid]);
				Map_Extract_To_Base_V.push_back(vid);//映射
				Visitflag[vid]=true;
			}

			newface[j]=Vnewid[vid];		
		}
		NewMesh->faces[i]=newface;
		Map_Extract_To_Base_F[i]=beselectF[i];


	}

	NewMesh->Is_clouds=false;
	ExtractMesh=NewMesh;
	return true;

}
void CToolFunction::FindBdy_3Ring(TriMesh *Tmesh)
{
	int vn=Tmesh->vertices.size();
	Tmesh->need_neighbors();
	vector<bool>bdyflag(vn,false);
	for (int i=0;i<vn;i++)
	{
		if (Tmesh->is_bdy(i))
		{
			bdyflag[i]=true;
			for (int j=0;j<Tmesh->neighbors[i].size();j++)
			{
				bdyflag[Tmesh->neighbors[i][j]]=true;
				int tworing=Tmesh->neighbors[i][j];
				for (int k=0;k<Tmesh->neighbors[tworing].size();k++)
				{
					bdyflag[Tmesh->neighbors[tworing][k]]=true;
				}
			}
		}
	}
	Tmesh->need_adjacentfaces();
	for (int i=0;i<vn;i++)
	{
		if (bdyflag[i]==true)
		{
			for (int j=0;j<Tmesh->adjacentfaces[i].size();j++)
			{
				int f0=Tmesh->adjacentfaces[i][j];
				Tmesh->faces[f0].beSelect=true;
			}

		}
	}

}
//按顺序搜索边界点函数,第二个参数是边界标志点,得到的边界环的第一个点是BoundaryFlag；
void CToolFunction::FindBorderV(TriMesh*Tmesh,int BoundaryFlag,vector<int>&BorderVertex)
{   
	Tmesh->need_adjacentedges();
	Tmesh->need_adjacentfaces();
	vector<int>tempboundary;
	vector<bool>visitflag(Tmesh->vertices.size(),false);

	BorderVertex.push_back(BoundaryFlag); 
	//保证按逆时针
	vector<int>&neighborf=Tmesh->adjacentfaces[BoundaryFlag];
	for (int i=0;i<neighborf.size();i++)
	{  
		for (int j=0;j<3;j++)
		{
			if (Tmesh->faces[neighborf[i]][j]==BoundaryFlag)
			{   
				int secondbdy=Tmesh->faces[neighborf[i]][(j+1)%3];
				if (Tmesh->is_bdy(secondbdy))
				{
					BorderVertex.push_back(secondbdy);
					tempboundary.push_back(secondbdy);
					visitflag[BoundaryFlag]=true;
					break;
				}
			}

		}
		if (visitflag[BoundaryFlag]==true)	break;			
	}


	vector<int>::iterator itbegin;
	while(tempboundary.size()>0)
	{   
		itbegin=tempboundary.begin();
		visitflag[*itbegin]=true;
		vector<int>&Nei=Tmesh->adjancetedge[*itbegin];
		for (int i=0;i<Nei.size();i++)
		{   
			::Tedge &a=Tmesh->m_edges[Nei[i]];
			if(a.m_adjacent_faces.size()==2)continue;
			long opsi=a.opposite_vertex(*itbegin);
			if (visitflag[opsi]==false)
			{
				tempboundary.push_back(opsi);
				BorderVertex.push_back(opsi);
				break;
			}
		}
		tempboundary.erase(tempboundary.begin());
	}
}
//按顺序搜索边界点函数,
vector<int> CToolFunction::FindBorderV(TriMesh*Tmesh)
{   
	Tmesh->need_adjacentedges();
	Tmesh->need_adjacentfaces();
	vector<int>BorderVertex;
	vector<int>tempboundary;
	vector<bool>visitflag(Tmesh->vertices.size(),false);
	for(int i=0;i<Tmesh->vertices.size();i++)
	{
		if(Tmesh->is_bdy(i))
		{   
			BorderVertex.push_back(i); 
			tempboundary.push_back(i);
			visitflag[i]=true;
			break;
		}
	}
	if(BorderVertex.empty())
	{
		return BorderVertex;

	}
		

	vector<int>::iterator itbegin;
	while(tempboundary.size()>0)
	{   
		itbegin=tempboundary.begin();
		visitflag[*itbegin]=true;
		vector<int>&Nei=Tmesh->adjancetedge[*itbegin];
		for (int i=0;i<Nei.size();i++)
		{   
			::Tedge &a=Tmesh->m_edges[Nei[i]];
			if(a.m_adjacent_faces.size()==2)continue;
			long opsi=a.opposite_vertex(*itbegin);
			if (visitflag[opsi]==false)
			{
				tempboundary.push_back(opsi);
				BorderVertex.push_back(opsi);
				break;
			}
		}
		tempboundary.erase(tempboundary.begin());
	}

	return BorderVertex;
}
//获取网格模型的XYZ各个方向的长度
/*vector<float> CToolFunction::Get_Mesh_Box_Length(TriMesh *Tmesh)
{
	vector<float>Box_XYZLength;
	TriMesh::BBox box;
	int l=0;
	vector<float> minxx;
	vector<float> minyy;
	vector<float> minzz;
	vector<float> maxxx;
	vector<float> maxyy;
	vector<float> maxzz;

		point minc=Tmesh->bbox.min;
		point maxc=Tmesh->bbox.max;
		minxx.push_back(minc[0]);
		minyy.push_back(minc[1]);
		minzz.push_back(minc[2]);

		maxxx.push_back(maxc[0]);
		maxyy.push_back(maxc[1]);
		maxzz.push_back(maxc[2]);
		l=l+1;


	int n=l;
	sort(minxx.begin(),minxx.end());
	sort(minyy.begin(),minyy.end());
	sort(minzz.begin(),minzz.end());
	point low(minxx[0],minyy[0],minzz[0]);
	sort(maxxx.begin(),maxxx.end());
	sort(maxyy.begin(),maxyy.end());
	sort(maxzz.begin(),maxzz.end());
	point hig(maxxx[n-1],maxyy[n-1],maxzz[n-1]);
	box.min=low;
	box.max=hig;
	return box;
}*/
//pt传入的是任意三维坐标，不一定是顶点坐标，然而通过KD树搜索离它最近的顶点就是所要选择的点,最后存入SelectedPoint
/*BOOL CToolFunction::SelPointMeshV(vec pt,vector<vec>v,int &vIndex)
{
	int vn=v.size();
	float *v0=&v[0][0];
	KDtree	*kd(v0, vn);

	float rad=50000.0;//只在100倍的平均边长的范围内搜索
	const float *match = kd->closest_to_pt(pt, rad);
	delete kd;
	if (match)
	{
		int my_id=(match - v0) / 3;
		point vp=mesh->vertices[my_id];
		vIndex=my_id;
		return TRUE;
	}
	return FALSE;
}*/
bool CToolFunction::FindNearestFace(TriMesh*Tmesh,point vi,int &Findex)
{

	Tmesh->need_adjacentfaces();
	int vn=Tmesh->vertices.size();
	float	*v0 = &Tmesh->vertices[0][0];
	KDtree	*kd = new KDtree(v0, vn);
	const float *match = kd->closest_to_pt(vi, 10.0f); 
	//寻找最近的顶点
	delete kd;
	point pmatch;
	int my_id=(match - v0) / 3;
	if(my_id<0||my_id>=vn)
	{
		Findex=-1;
		return false;
	}

	point vp=Tmesh->vertices[my_id];
	//寻找最近的三角面片，及在此三角面片上的投影顶点
	const vector<int>&a=Tmesh->adjacentfaces[my_id];
	int tri_id=-1;
	float closest_dist2=10000;
	for (unsigned i=0;i<a.size();i++)
	{
		point c=closest_on_face(Tmesh,a[i],vi);
		float this_dist2=dist2(c,vi);
		if (this_dist2<closest_dist2)
		{
			closest_dist2=this_dist2;
			tri_id=a[i];
		}
	}

	Findex=tri_id;
	return true;
}
bool CToolFunction::PointonMesh(vec vpoint,TriMesh*Tmesh,CVertexOnMesh &SelectV)
{
	if(Tmesh==NULL) return false;
	Tmesh->need_adjacentfaces();
	int NearestV=0;
	if (NearestPointMeshV(vpoint,Tmesh,NearestV))
	{
		point pmatch;
		int ind=NearestV;
		const vector<int> &a = Tmesh->adjacentfaces[ind];
		if (a.empty()) //孤立顶点？
		{
			return false;
		}

		point pp=vpoint;
		int tri_id=-1;

		float closest_dist2 = 3.3e33;
		for (int i = 0; i < a.size(); i++) 
		{
			point c = closest_on_face(Tmesh, a[i], pp);
			float this_dist2 = dist2(c, pp);
			if (this_dist2 < closest_dist2) 
			{
				closest_dist2 = this_dist2;
				pmatch = c;
				tri_id=a[i];
			}
		}
		if (closest_dist2 != 3.3e33)
		{
			CVertexOnMesh pm;
			pm.face_id=tri_id;
			pm.vp=pmatch;
			double u,v,w;
			Calcuvw(pmatch, Tmesh->vertices[Tmesh->faces[tri_id][0]] , Tmesh->vertices[Tmesh->faces[tri_id][1]], Tmesh->vertices[Tmesh->faces[tri_id][2]],u,v,w);
			pm.uvw[0]=u;
			pm.uvw[1]=v;
			pm.uvw[2]=w;
			SelectV=pm;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
bool CToolFunction::NearestPointMeshV(vec vi,TriMesh*Tmesh,int &vIndex)
{
	if(Tmesh==NULL) 
	{
		return false;
	}
	Tmesh->need_use_KDTree();
	float	*v0=&Tmesh->vertices[0][0];
	const float *match = Tmesh->m_Tmesh_KD->closest_to_pt(vi);
	if (match)
	{
		vIndex=(match - v0) / 3;
		return true;
	}
	return false;
}


