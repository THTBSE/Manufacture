/*
TriMesh_connectivity.cpp
Manipulate data structures that describe connectivity between faces and verts.
*/

#include <stdio.h>
#include "..\include\TriMesh.h"
#include "..\include\KDtree.h"
#include <algorithm>
#include <queue>
#include <map>
#include <set>
using std::queue;
using std::find;


// Find the direct neighbors of each vertex
void TriMesh::need_neighbors()
{
	if (!neighbors.empty())
		return;

	need_faces();
	if (faces.empty())
		return;

	printf("Finding vertex neighbors... ");
	int nv = vertices.size(), nf = faces.size();

	vector<int> numneighbors(nv);
	for (int i = 0; i < nf; i++) {
		numneighbors[faces[i][0]]++;
		numneighbors[faces[i][1]]++;
		numneighbors[faces[i][2]]++;
	}

	neighbors.resize(nv);
	for (int i = 0; i < nv; i++)
		neighbors[i].reserve(numneighbors[i]+2); // Slop for boundaries

	//通过遍历每个面片上的三个顶点，因为是已经去除冗余的三角网格
	//所以，选定一个顶点，则两个两个顶点必然是该顶点的邻域
	//只要查询一下这两个顶点的索引有没有已经被添加进数组
	//若没有，则添加
	//只要面片包含的顶点索引能正确索引到该顶点，面片在数组里摆放的
	//顺序并不重要，都能正确的建立拓扑关系

	for (int i = 0; i < nf; i++) {
		for (int j = 0; j < 3; j++) {
			vector<int> &me = neighbors[faces[i][j]];
			int n1 = faces[i][(j+1)%3];
			int n2 = faces[i][(j+2)%3];
			if (find(me.begin(), me.end(), n1) == me.end())
				me.push_back(n1);
			if (find(me.begin(), me.end(), n2) == me.end())
				me.push_back(n2);
		}
	}

	printf("Done.\n");
}


// Find the faces touching each vertex
void TriMesh::need_adjacentfaces()
{
	if (!adjacentfaces.empty())
		return;

	need_faces();
	if (faces.empty())
		return;

	printf("Finding vertex to triangle maps... ");
	int nv = vertices.size(), nf = faces.size();

	//numadjacentfaces 用来统计每个顶点被多少个面所共有
	vector<int> numadjacentfaces(nv);
	for (int i = 0; i < nf; i++) {
		numadjacentfaces[faces[i][0]]++;
		numadjacentfaces[faces[i][1]]++;
		numadjacentfaces[faces[i][2]]++;
	}

	//将adjacentfaces的size设置为顶点的size
	adjacentfaces.resize(vertices.size());
	for (int i=0; i<nv; i++)
		adjacentfaces[i].reserve(numadjacentfaces[i]);   //为每个顶点预留出共有它们的面片的个数的空间
	//reserve这步写不写都一样，这么写程序的性能会更好
	for (int i=0; i<nf; i++) {
		for (int j=0; j<3; j++)
			adjacentfaces[faces[i][j]].push_back(i);    //每个面片上的顶点必然被索引到它的这个面片包含，全部遍历
	}                                                   //就可以得到每个顶点相邻的面片索引
	printf("Done.\n");
}

// Find the face across each TriEdge from each other face (-1 on boundary)
// If topology is bad, not necessarily what one would expect...
void TriMesh::need_across_edge()
{
	if (!across_edge.empty())
		return;

	need_adjacentfaces();
	if (adjacentfaces.empty())
		return;

	printf("Finding across-TriEdge maps... ");

	int nf = faces.size();
	across_edge.resize(nf, Face(-1,-1,-1));

	for (int i = 0; i < nf; i++) 
	{
		for (int j = 0; j < 3; j++) 
		{
			if (across_edge[i][j] != -1)
				continue;
			int v1 = faces[i][(j+1)%3];
			int v2 = faces[i][(j+2)%3];
			const vector<int> &a1 = adjacentfaces[v1];
			const vector<int> &a2 = adjacentfaces[v2];
			for (unsigned k1 = 0; k1 < a1.size(); k1++) 
			{
				int other = a1[k1];
				if (other == i)
					continue;
				vector<int>::const_iterator it = find(a2.begin(), a2.end(), other);
				if (it == a2.end())
					continue;
				int ind = (faces[other].indexof(v1)+1)%3;
				if (faces[other][(ind+1)%3] != v2)
					continue;
				across_edge[i][j] = other;            
				across_edge[other][ind] = i;
				break;
			}
		}
	}

	printf("Done.\n");
}

typedef std::pair<int,int> Pair;
typedef std::map<Pair,int> mapEdges;

void TriMesh::need_edges()
{
	if (!m_edges.empty())
		return;

	int fsize = static_cast<int>(faces.size());
	m_edges.reserve(3 * fsize);
	mapEdges mEs;
	mapEdges::iterator mEsIter;
	int eNum = 0;
	TriEdge baseEdge;
	facet_edges.resize(fsize);
	for (int i=0; i<fsize; i++)
	{
		faces[i]._id = i;
		for (int j=0; j<3; j++)
		{
			int vCurr = std::min(faces[i][j],faces[i][(j+1)%3]);
			int vNext = std::max(faces[i][j],faces[i][(j+1)%3]);
			mEsIter = mEs.find(Pair(vCurr, vNext));
			if (mEsIter == mEs.end())
			{
				facet_edges[i].push_back(eNum);
				mEs.insert(mapEdges::value_type(Pair(vCurr,vNext),eNum++));
				m_edges.push_back(baseEdge);
				m_edges.back().m_adjacent_faces.push_back(faces[i]);
				m_edges.back().m_adjacent_vertices[0] = vCurr;
				m_edges.back().m_adjacent_vertices[1] = vNext;
				m_edges.back().m_id = m_edges.size() - 1;
				m_edges.back().length() = len(vertices[m_edges.back().m_adjacent_vertices[0]]-vertices[m_edges.back().m_adjacent_vertices[1]]);
				assert(m_edges.back().length() > 1e-100);		//algorithm works well with non-degenerate meshes only 

			}
			else
			{
				int eid = mEsIter->second;
				m_edges[eid].m_adjacent_faces.push_back(faces[i]);
				facet_edges[i].push_back(eid);
			}
		}
	}
}

void TriMesh::build_adjacencies()            
{
	if (!m_edges.empty())
		return;

	for (size_t l=0;l<faces.size();l++)
	{
		faces[l]._id=l;
	}

	//find all edges
	//i.e. find all half-edges, sort and combine them into edges
	std::vector<HalfEdge> half_edges(faces.size() * 3);

	int k = 0;
	for(size_t i=0; i<faces.size(); ++i)
	{
		Face& f = faces[i];
		for(int j = 0; j < 3; ++j)
		{
			half_edges[k].face_id = i;
			int vertex_id_1 = f[j];
			int vertex_id_2 = f[(j+1) % 3];
			half_edges[k].vertex_0 = std::min(vertex_id_1, vertex_id_2);
			half_edges[k].vertex_1 = std::max(vertex_id_1, vertex_id_2);
			k++;
		}
	}
	//let same half_edge stay together 
	std::sort(half_edges.begin(), half_edges.end());
	
	int number_of_edges = 1;
	for(size_t i=1; i<half_edges.size(); ++i)
	{
		if(half_edges[i] != half_edges[i-1])
		{
			++number_of_edges;
		}
		else
		{
			if(i < half_edges.size()-1)		//sanity check: there should be at most two equal half-edges
			{								//if it fails, most likely the input data are messed up
				assert(half_edges[i] != half_edges[i+1]);
			}
		}
	}

	//		Edges->adjacent Vertices and Faces
	facet_edges.resize(faces.size());
	m_edges.resize(number_of_edges);
	int edge_id = 0;
	for(size_t i=0; i<half_edges.size();)
	{
		TriEdge& e = m_edges[edge_id];
		e.m_id = edge_id++;

		e.m_adjacent_vertices[0] = half_edges[i].vertex_0;
		e.m_adjacent_vertices[1] = half_edges[i].vertex_1;
		
		e.length() = len(vertices[e.m_adjacent_vertices[0]]-vertices[e.m_adjacent_vertices[1]]);
		assert(e.length() > 1e-100);		//algorithm works well with non-degenerate meshes only 

		if(i != half_edges.size()-1 && half_edges[i] == half_edges[i+1])	//double TriEdge
		{
			e.m_adjacent_faces.push_back(faces[half_edges[i].face_id]) ;
			facet_edges[half_edges[i].face_id].push_back(e.m_id);
			e.m_adjacent_faces.push_back(faces[half_edges[i+1].face_id]) ;
			facet_edges[half_edges[i+1].face_id].push_back(e.m_id);
			i += 2;
		}
		else			//single TriEdge
		{			//one adjacent faces
			e.m_adjacent_faces.push_back(faces[half_edges[i].face_id]) ;
			facet_edges[half_edges[i].face_id].push_back(e.m_id);
			i += 1;
		}
	}

}


//2009.3.9为模型上的局部区域选择服务，方法见Metamorphosis of Arbitrary Triangular Meshes 一文
void TriMesh::build_halfedge()
{
	halfedges_for_face.resize(faces.size());
	for(unsigned int i=0; i<faces.size(); ++i)
	{
		Face& f = faces[i];
		for(int j=0; j<3; ++j)
		{
			long vertex_id_1 = f[j];
			long vertex_id_2 = f[(j+1) % 3];

			HalfEdge temp;
			temp.face_id=i;
			temp.vertex_0=vertex_id_1;
			temp.vertex_1=vertex_id_2;
			halfedges_for_face[i].push_back(temp);
		}
	}
}
//bj存储一系列按照逆时针排序的顶点，构成所选区域的边界
void TriMesh::zone_select(vector<int>& bj, vector<int>& zone, vector<int>& SelFaces)
{
	build_halfedge();
	build_adjacencies();
	need_neighbors();
	need_adjacentfaces();

	vector<int>  v_in_sel;
	vector<bool> beSel(vertices.size(),false);

	int n=bj.size();
	for (int i=0; i<n;i++)
	{
		int z=bj[i];
		beSel[z]=true;
		v_in_sel.push_back(z);
	}

	bool seed_face_id=false;
	int  seed_v_bsel=-1;   //选择区域内的一个点，作为广度优先搜索的种子点
	for (int i=0; i<n-1;i++)
	{
		point v0=vertices[bj[i]];
		point v1=vertices[bj[i+1]];

		for (unsigned int j=0;j<faces.size();j++)
		{
			for (int k=0;k<3;k++)
			{
				int cp0=halfedges_for_face[j][k].vertex_0;
				int cp1=halfedges_for_face[j][k].vertex_1;
				if (vertices[cp0]==v0 && vertices[cp1]==v1)
				{
					//faces[j]被标记为与边v0v1相连的左侧三角面，三角面的法矢必须向外
					for (int m=0;m<3;m++)
					{
						int v=faces[j][m];
						if (vertices[v]!=v0 && vertices[v]!=v1)
						{
							if(beSel[v]) continue;   //种子点就是边界点的情况
							seed_v_bsel=v;        //找到一个种子点，跳出循环
							seed_face_id=true;
							break;
						}
					}
					break;
				}
			}
			if(seed_face_id) 
				break;
		}
		if(seed_face_id) break;
	}

	//广度优先搜索
	if(seed_v_bsel!=-1)
	{
		beSel[seed_v_bsel] = true;
		v_in_sel.push_back(seed_v_bsel);
		vector<int> nearby;
		// add all the neighbors of the starting vertex into nearby
		for(unsigned int j=0;j< neighbors[seed_v_bsel].size();j++)
		{
			int nid=neighbors[seed_v_bsel][j];
			nearby.push_back(nid);
		}

		// repeat until nearby is empty:
		while(nearby.size()>0)
		{
			// for each nearby point:
			int iNearby,iNeighbor;
			for(unsigned int i=0; i<nearby.size(); i++)
			{
				iNearby = nearby[i];
				if(beSel[iNearby]) 
				{
					vector<int>::iterator iter;
					iter=find(nearby.begin(),nearby.end(),iNearby);
					nearby.erase(iter);
					continue;
				}
				beSel[iNearby] = true;
				v_in_sel.push_back(iNearby);

				vector<int>::iterator iter;
				iter=find(nearby.begin(),nearby.end(),iNearby);
				nearby.erase(iter);

				for(unsigned int j=0;j<neighbors[iNearby].size();j++)
				{
					iNeighbor = neighbors[iNearby][j];
					if(beSel[iNeighbor]) continue;
					nearby.push_back(iNeighbor);
				}
			}
		}
	}
	int nn=v_in_sel.size();
	zone.clear();
	for (int l=0;l<nn; l++)
	{
		int x=v_in_sel[l];
		zone.push_back(x);
	}
	int nf=faces.size();
	vector<bool> fangwen(nf, false);
	SelFaces.clear();
	for (int l=0;l<nn; l++)
	{
		int x=v_in_sel[l];
		const vector<int> &a = adjacentfaces[x];
		if (a.empty()) continue;
		for (unsigned int j=0; j<a.size(); j++)
		{
			int t=a[j];
			if(fangwen[t]) continue;
			bool flag=false;
			for (int k=0;k<3; k++)
			{
				if (!beSel[faces[t].v[k]])
				{
					flag=true;       //边界外三角形
					break;;
				}
			}
			if(flag) continue; 
			SelFaces.push_back(t);
			fangwen[t]=true;
		}
	}
}

void bfs_mesh_select(TriMesh* mesh, std::vector<int>& boundary, int seed)
{
	if (!mesh)
		return;
	mesh->need_neighbors();
	mesh->need_adjacentfaces();
	std::set<int> borderPoint(boundary.begin(), boundary.end());
	std::set<int> traversed;
	queue<int> bfsPoints;
	bfsPoints.push(seed);
	traversed.insert(seed);

	while (!bfsPoints.empty())
	{
		int pt = bfsPoints.front();
		bfsPoints.pop();

		for_each(mesh->adjacentfaces[pt].begin(), mesh->adjacentfaces[pt].end(),
			[mesh](int i){
			if (!mesh->faces[i].beSelect)
			{
				mesh->faces[i].beSelect = true;
			}

		});
		for_each(mesh->neighbors[pt].begin(), mesh->neighbors[pt].end(),
			[&](int i){
			if (!borderPoint.count(i) && !traversed.count(i))
			{
				traversed.insert(i);
				bfsPoints.push(i);
			}
		});
	}
}