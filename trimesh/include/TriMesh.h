#ifndef TRIMESH_H
#define TRIMESH_H
/*
TriMesh.h
Class for triangle meshes.
*/
#include <assert.h>
#include "Vec.h"
#include "Color.h"
#include <vector>
#include <limits>
#include "XForm.h"
#include <queue>
using std::queue;
using std::vector;
class TriMesh;
class TriEdge;
#ifndef M_PIf
# define M_PIf 3.1415927f
#endif

//***************************for 测地线*******************
//-------------------------------------------------------------------------
struct HalfEdge			//prototype of the TriEdge; used for mesh construction
{
	long face_id;
	long vertex_0;		//adjacent vertices sorted by id value
	long vertex_1;		//they are sorted, vertex_0 < vertex_1
};

inline bool operator < (const HalfEdge &x, const HalfEdge &y)
{
	if(x.vertex_0 == y.vertex_0)
	{
	    return x.vertex_1 < y.vertex_1;
	}
	else
	{
		return x.vertex_0 < y.vertex_0;
	}
}

inline bool operator != (const HalfEdge &x, const HalfEdge &y)
{
	return x.vertex_0 != y.vertex_0 || x.vertex_1 != y.vertex_1;
}

inline bool operator == (const HalfEdge &x, const HalfEdge &y)
{
	return x.vertex_0 == y.vertex_0 && x.vertex_1 == y.vertex_1;
}
//*********************************************************************
struct PartColor
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	PartColor():r(180),g(180),b(180)
	{
	}
};
class TriMesh {
protected:
	static bool read_helper(const char *filename, TriMesh *mesh);
	string		m_name;
	bool		m_bVisible;
	string		m_fpath;  //文件路径

public:
	// Types
	struct Face 
	{
		int		v[3];
		bool	beSelect;  //是否被选中
		bool    beUsed;    //是否在建立截面与截面间拓扑关系时被遍历
		long	_id;

		Face() {beSelect = false; _id = -1;beUsed = false; }
		Face(const int &v0, const int &v1, const int &v2)
			{ v[0] = v0; v[1] = v1; v[2] = v2; beSelect=false; beUsed = false;}
		Face(const int *v_)
			{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; beSelect=false; beUsed = false; }
		int &operator[] (int i) { return v[i]; }
		const int &operator[] (int i) const { return v[i]; }
		operator const int * () const { return &(v[0]); }
		operator const int * () { return &(v[0]); }
		operator int * () { return &(v[0]); }
		int indexof(int v_) const
		{
			return (v[0] == v_) ? 0 :
			       (v[1] == v_) ? 1 :
			       (v[2] == v_) ? 2 : -1;
		}
	};

	class BBox 
	{
	public:
		point min, max;
		bool valid;

		// Construct as empty
		BBox() : min(point(std::numeric_limits<float>::max(),
		                   std::numeric_limits<float>::max(),
		                   std::numeric_limits<float>::max())),
		         max(point(std::numeric_limits<float>::min(),
		                   std::numeric_limits<float>::min(),
		                   std::numeric_limits<float>::min())),
			     valid(false)
			{}

		// Initialize to one point or two points
		BBox(const point &p) : min(p), max(p), valid(true)
			{}
		BBox(const point &min_, const point &max_) :
			min(min_), max(max_), valid(true)
			{}

		// Mark invalid
		void clear()
		{
			min = point(std::numeric_limits<float>::max(),
		                    std::numeric_limits<float>::max(),
		                    std::numeric_limits<float>::max());
			max = point(std::numeric_limits<float>::min(),
		                    std::numeric_limits<float>::min(),
		                    std::numeric_limits<float>::min());
			valid = false;
		}

		// Return center point and (vector) diagonal
		point center() const { return 0.5f * (min+max); }
		vec size() const { return max - min; }

		// Grow a bounding box to encompass a point
		BBox &operator += (const point &p)
			{ min.min(p); max.max(p); return *this; }
		BBox &operator += (const BBox &b)
			{ min.min(b.min); max.max(b.max); return *this; }

		// The following appear to be necessary for Visual Studio,
		// despite the fact that the operators shouldn't need
		// to be friends...
		friend const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p);
		friend const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b);
		friend const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2);
	};

	struct BSphere 
	{
		point center;
		float r;
		bool valid;
		BSphere() : valid(false)
			{}
	};

	// Enums
	enum tstrip_rep { TSTRIP_LENGTH, TSTRIP_TERM };
	enum { GRID_INVALID = -1 };

	// The basics: vertices and faces
	vector<point>			vertices;
	vector<Face>			faces;
	vector<vec>				FaceNormal;         //三角形的法矢
	bool				    Is_clouds;          //是否点云数据
	xform				    form;				//保存模型的转换矩阵
	PartColor				me_color;
	int						My_ID;              //索引

	vector<bool>			besel;			    //点云模型顶点是否被索引

	// Triangle strips
	vector<int>				tstrips;

	// Grid, if present
	vector<int>				grid;
	int					    grid_width, grid_height;

	// Other per-vertex properties
	vector<Color>			colors;
	vector<float>			confidences;
	vector<unsigned>		flags;
	unsigned				flag_curr;

	// Computed per-vertex properties
	vector<vec>				normals;
	vector<vec>				pdir1, pdir2;
	vector<float>			curv1, curv2;
	vector< Vec<4,float> >	dcurv;
	vector<vec>				cornerareas;
	vector<float>			pointareas;

	// Bounding structures
	BBox				    bbox;
	BSphere					bsphere;

	// Connectivity structures:
	//  For each vertex, all neighboring vertices
	vector< vector<int> >	neighbors;
	//  For each vertex, all neighboring faces
	vector< vector<int> >	adjacentfaces;
	//  For each face, the three faces attached to its edges
	//  (for example, across_edge[3][2] is the number of the face
	//   that's touching the TriEdge opposite vertex 2 of face 3)
	//   就是说除了面[3]中点[2]的另外两点构成的边所相邻的三角形面片的索引
	//   across_edge 也就保存了一个面片的相邻三角形面片的索引
	vector<Face>			across_edge;
	//For each face, the three edges index of it , should call 'build_adjacencies'
	//to get these stuff .. 
	vector<vector<int> >    facet_edges;
	// Compute all this stuff...
	void							need_tstrips();
	void							convert_strips(tstrip_rep rep);
	void							unpack_tstrips();
	void							triangulate_grid();
	void							need_faces()
	{
		if (!faces.empty())
			return;
		if (!tstrips.empty())
			unpack_tstrips();
		else if (!grid.empty())
			triangulate_grid();
	}

	void                            reverse_normals();
	void							need_normals();
	void							need_Facenormals();
	void							need_pointareas();
	void							need_curvatures();
	void							need_dcurv();
	void							need_bbox();
	void							need_bsphere();
	void							need_neighbors();
	void							need_adjacentfaces();
	void							need_across_edge();
	void							remove_outliers( float threshold_percent);
	void							need_edges();

	// Input and output
	static TriMesh *read(const char *filename);
	bool write(const char *filename);
	static TriMesh *read_stl( const char* filename);
	static TriMesh *read_pts(string fname);
	static TriMesh *load_stl( const char *filename);	//读取二进制格式STL文件
	static TriMesh *load_stl2( const char *filename);
	bool   write_stl(const char* fname);
	static TriMesh* read_stl_helper(const char* filename);

	// Statistics
	// XXX - Add stuff here
	float feature_size();
	float need_rou();  //点云模型中点与点之间的平均距离


	// Useful queries
	// Is vertex v on the mesh boundary?
	bool is_bdy(int v)
	{
		if (neighbors.empty()) need_neighbors();
		if (adjacentfaces.empty()) need_adjacentfaces();
		return neighbors[v].size() != adjacentfaces[v].size();
	}

	// Centroid of face f
	vec centroid(int f)
	{
		if (faces.empty()) need_faces();
		return (1.0f / 3.0f) *
			(vertices[faces[f][0]] +
			vertices[faces[f][1]] +
			vertices[faces[f][2]]);
	}

	// Normal of face f
	vec trinorm(int f)
	{
		if (faces.empty()) need_faces();
		return ::trinorm(vertices[faces[f][0]], vertices[faces[f][1]],
			vertices[faces[f][2]]);
	}

	// Angle of corner j in triangle i
	float cornerangle(int i, int j)
	{
		using namespace ::std;

		if (faces.empty()) need_faces();
		const point &p0 = vertices[faces[i][j]];
		const point &p1 = vertices[faces[i][(j+1)%3]];
		const point &p2 = vertices[faces[i][(j+2)%3]];
		return acos((p1 - p0) DOT (p2 - p0));
	}

	// Dihedral angle between face i and face across_edge[i][j]
	float dihedral(int i, int j)
	{
		if (across_edge.empty()) need_across_edge();
		if (across_edge[i][j] < 0) return 0.0f;
		vec mynorm = trinorm(i);
		vec othernorm = trinorm(across_edge[i][j]);
		float ang = angle(mynorm, othernorm);
		vec towards = 0.5f * (vertices[faces[i][(j+1)%3]] +
			vertices[faces[i][(j+2)%3]]) -
			vertices[faces[i][j]];
		if ((towards DOT othernorm) < 0.0f)
			return M_PIf + ang;
		else
			return M_PIf - ang;
	}

	float dihedral_m(int i,int j)
	{
		if (across_edge.empty()) need_across_edge();
		if (across_edge[i][j] < 0) return 0.0f;
		vec mynorm = trinorm(i);
		vec othernorm = trinorm(across_edge[i][j]);
		float ang = angle(mynorm, othernorm);
		return ang;
	}

	// Debugging printout, controllable by a "verbose"ness parameter
	static int verbose;
	static void set_verbose(int);
	//-------------------------------------------
	void SetName(string name)
	{
		m_name=name;
	}
	string GetName()
	{
		return m_name;
	}
	void SetPath(string mpath)
	{
		m_fpath=mpath;
	}
	string Getpath()
	{
		return m_fpath;
	}
	bool IsVisible(){return m_bVisible;}
	void SetVisible(bool bVisible){m_bVisible=bVisible;}

	//-------------------------------------------

	// Constructor
	TriMesh() : grid_width(-1), grid_height(-1), flag_curr(0), Is_clouds(false),m_bVisible(true),My_ID(-1)
		{}
	//-----------------------------------------------2010.5.28
	void											build_adjacencies();
	vector<TriEdge>							        m_edges;
	vector< vector<HalfEdge> >		                halfedges_for_face;
	void											build_halfedge();  //为模型上局部区域选择服务
	void											zone_select(vector<int>& bj,  vector<int>& zone, vector<int>& SelFaces);
 
};

inline const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p)
{
	return TriMesh::BBox(b) += p;
}

inline const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b)
{
	return TriMesh::BBox(b) += p;
}

inline const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2)
{
	return TriMesh::BBox(b1) += b2;
}
//--------------------------------------------------------------------------
class TriEdge
{
public:
	typedef TriMesh::Face face_pointer;

	TriEdge()
	{
		established = false;
		m_id=0;
		m_adjacent_vertices.resize(2);
		m_adjacent_faces.reserve(2);
	}

	~TriEdge(){}

	double& length(){return m_length;}

	face_pointer opposite_face(face_pointer f)
	{
		if(m_adjacent_faces.size() == 1)
		{
			assert(m_adjacent_faces[0]._id == f._id);
			return NULL;
		}

		assert(m_adjacent_faces[0]._id == f._id || 
			m_adjacent_faces[1]._id == f._id);

		return m_adjacent_faces[0]._id == f._id ? 
			m_adjacent_faces[1] : m_adjacent_faces[0];
	}

	int opposite_face(int i)
	{
	    if (m_adjacent_faces.size() < 2)
		    return -1;
	    else
	    {
	        return m_adjacent_faces[0]._id == i ?
		       m_adjacent_faces[1]._id : m_adjacent_faces[0]._id;
	    }
    }

	int opposite_vertex(int v)
	{
		assert(belongs(v));

		return m_adjacent_vertices[0] == v ?
			m_adjacent_vertices[1] : m_adjacent_vertices[0];
	}

	bool belongs(int v)
	{
		return m_adjacent_vertices[0] == v || 
			m_adjacent_vertices[1] == v;
	}

	bool is_boundary(){return m_adjacent_faces.size() == 1;}

	long v0(){return m_adjacent_vertices[0];}
	long v1(){return m_adjacent_vertices[1];}

	void local_coordinates(point pt, 
		double& x, 
		double& y, TriMesh tmesh)
	{
		double d0 = len(pt-tmesh.vertices[v0()]);
		if(d0 < 1e-50)
		{
			x = 0.0;
			y = 0.0;
			return;
		}

		double d1 = len(pt-tmesh.vertices[v1()]);
		if(d1 < 1e-50)
		{
			x = m_length;
			y = 0.0;
			return;
		}

		x = m_length/2.0 + (d0*d0 - d1*d1)/(2.0*m_length);
		y = sqrt(__max(0.0, d0*d0 - x*x));
		return;
	}
public:
	vector<face_pointer>	    m_adjacent_faces;			//list of the adjacent faces
	vector<int>					m_adjacent_vertices;		//list of the adjacent vertices
	vector<int>                 m_adjacent_edges;
	int							m_id;							//unique id
	bool                        established;

private:
	double						m_length;					//length of the TriEdge
};

#endif
