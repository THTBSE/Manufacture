/*
mesh_shade.cpp
Apply procedural shaders to a mesh
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include <float.h>
#include <algorithm>
#include "..\include\TriMesh.h"
#include "..\include\TriMesh_algo.h"
#include "..\include\KDtree.h"
#include "..\include\lineqn.h"
using namespace std;

#define BIGNUM 3.3e33

#ifdef WIN32
# ifndef strncasecmp
#  define strncasecmp strnicmp
# endif
#endif

#define isfinite( x ) ( x <= FLT_MAX ) 
#define M_2_PI  M_PI*2.0
#define M_PI_2  M_PI/2.0
int m_flag=-1;
// Quick 'n dirty portable random number generator 
static inline float tinyrnd()
{
	// Assumes unsigned is exactly 32 bits
	static unsigned trand = 0;
	trand = 1664525u * trand + 1013904223u;
	return (float) trand / 4294967296.0f;
}


// Apply a solid color to the mesh
void solidcolor(TriMesh *mesh, const char *col)
{
	unsigned c;
	sscanf(col, "%x", &c);
	int r = (c >> 16) & 0xff;
	int g = (c >> 8)  & 0xff;
	int b =  c        & 0xff;
	Color cc = Color(r,g,b);
	int nv = mesh->vertices.size();
	for (int i = 0; i < nv; i++)
		mesh->colors[i] = cc;
}

// Color based on normals
void colorbynormals(TriMesh *mesh)
{
	mesh->need_normals();
	int nv = mesh->vertices.size();
	for (int i = 0; i < nv; i++) 
	{
		mesh->colors[i] = Color(0.5f, 0.5f, 0.5f) +
			0.5f * mesh->normals[i];
	}
}

// Compute a "feature size" for the mesh: computed as 1% of 
// the reciprocal of the 10-th percentile curvature 
float feature_size(TriMesh *mesh)
{
	mesh->need_curvatures();
	int nv = mesh->curv1.size();
	int nsamp = min(nv, 500);
                
	vector<float> samples;
	samples.reserve(nsamp * 2);
        
	for (int i = 0; i < nsamp; i++) 
	{
		// Quick 'n dirty portable random number generator  
		static unsigned randq = 0;
		randq = unsigned(1664525) * randq + unsigned(1013904223);

		int ind = int(tinyrnd() * nv);
		samples.push_back(fabs(mesh->curv1[ind]));
		samples.push_back(fabs(mesh->curv2[ind]));
	}
	const float frac = 0.1f;
	const float mult = 0.01f;
	int which = int(frac * samples.size());
	nth_element(samples.begin(), samples.begin() + which, samples.end());
	float f = mult / samples[which];
	if (!isfinite(f)) 
	{
		mesh->need_bsphere();
		f = mesh->bsphere.r;
	}
		
	return f;
}

// Color based on curvature
void colorbycurv(TriMesh *mesh, const char *scale, const char *smooth)
{
	mesh->need_curvatures();
	float smoothsigma = atof(smooth);
	if (smoothsigma > 0.0f) 
	{
		smoothsigma *= mesh->feature_size();
		diffuse_curv(mesh, smoothsigma);
	}
	float cscale = 10.0f * atof(scale) * feature_size(mesh);
	cscale = sqr(cscale);

	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		float H = 0.5f * (mesh->curv1[i] + mesh->curv2[i]);
		float K = mesh->curv1[i] * mesh->curv2[i];
		float h = 4.0f / 3.0f * fabs(atan2(H*H-K,H*H*sgn(H)));
		float s = M_2_PI * atan((2.0f*H*H-K)*cscale);
		mesh->colors[i] = Color::hsv(h,s,1.0);
	}
}

// Color based on curvature.  Similar to above, but uses a grayscale mapping.
void gcolorbycurv(TriMesh *mesh, const char *scale, const char *smooth)
{
	mesh->need_curvatures();
	float smoothsigma = atof(smooth);
	if (smoothsigma > 0.0f) 
	{
		smoothsigma *= mesh->feature_size();
		diffuse_curv(mesh, smoothsigma);
	}
	float cscale = 10.0f * atof(scale) * feature_size(mesh);

	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		float H = 0.5f * (mesh->curv1[i] + mesh->curv2[i]);
		mesh->colors[i] = Color(float((atan(H*cscale) + M_PI_2) / M_PI));
	}
}

// Accessibility shading
void acc(TriMesh *mesh, const char *maxsize_, const char *offset_)
{
	mesh->need_normals();
	float fs = feature_size(mesh);
	float maxsize = atof(maxsize_) * fs;
	float offset = atof(offset_) * fs;

	KDtree *kd = new KDtree(mesh->vertices);
	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		const vec &n = mesh->normals[i];
		point p = mesh->vertices[i] + offset * n;
		float tmin = 0, tmax = maxsize;
		for (int iter = 0; iter < 8; iter++) 
		{
			float tmid = 0.5f * (tmin + tmax);
			point q = p + tmid * n;
			const float *qq = kd->closest_to_pt(q, sqr(tmid));
			if (qq)
				tmax = tmid;
			else
				tmin = tmid;
		}
		mesh->colors[i] = Color(0.5f * (tmin + tmax));
	}
	delete kd;
}

// Color by distance to bdy
void bdyshade(TriMesh *mesh, const char *nedges_)
{
	int nedges = atoi(nedges_) + 1;
	int nv = mesh->vertices.size();
	mesh->need_neighbors();
	mesh->flags.resize(nv);
	for (int i = 0; i < nv; i++)
		mesh->flags[i] = mesh->is_bdy(i) ? 0 : nedges;
	for (int iter = 1; iter < nedges; iter++) 
	{
		for (int i = 0; i < nv; i++)
		{
			for (unsigned int j = 0; j < mesh->neighbors[i].size(); j++) 
			{
				int n = mesh->neighbors[i][j];
				if (mesh->flags[n] + 1 < mesh->flags[i])
					mesh->flags[i] = mesh->flags[n] + 1;
			}
		}
	}
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		float c = (float) mesh->flags[i] / nedges;
		mesh->colors[i] = Color(1.0f, c, c);
	}
}

// Helper for dist2mesh:
// Find closest point to p on segment from v0 to v1
point closest_on_segment(const point &v0, const point &v1, const point &p)
{
	vec v01 = v1 - v0;
	float d = (p - v0) DOT v01;
	d /= len2(v01);
	if (d < 0.0f)
		d = 0.0f;
	else if (d > 1.0f)
		d = 1.0f;
	return v0 + d * v01;
}

// Helper for dist2mesh:
// Find closest point to p on face i of mesh
point closest_on_face(const TriMesh *mesh, int i, const point &p)
{
	const TriMesh::Face &f = mesh->faces[i];
	const point &v0 = mesh->vertices[f[0]];
	const point &v1 = mesh->vertices[f[1]];
	const point &v2 = mesh->vertices[f[2]];
	vec a = v1 - v0, b = v2 - v0, p1 = p - v0, n = a CROSS b;

	float A[3][3] = { { a[0], b[0], n[0] },
			  { a[1], b[1], n[1] },
			  { a[2], b[2], n[2] } };
	float x[3] = { p1[0], p1[1], p1[2] };
	int indx[3];
	ludcmp<float,3>(A, indx);
	lubksb<float,3>(A, indx, x);

	if (x[0] >= 0.0f && x[1] >= 0.0f && x[0] + x[1] <= 1.0f)
		return v0 + x[0] * a + x[1] * b;

	point c01 = closest_on_segment(v0, v1, p);
	point c12 = closest_on_segment(v1, v2, p);
	point c20 = closest_on_segment(v2, v0, p);
	float d01 = dist2(c01, p);
	float d12 = dist2(c12, p);
	float d20 = dist2(c20, p);
	if (d01 < d12) 
	{
		if (d01 < d20) return c01; else return c20;
	} else 
	{
		if (d12 < d20) return c12; else return c20;
	}
}

// Helper for dist2mesh:
// Find (good approximation to) closest point on mesh to p.
// Finds closest vertex, then checks all faces that touch it.
bool find_closest_pt(const TriMesh *mesh, const KDtree *kd, const point &p,
		     float maxdist2, point &pmatch)
{
	const float *match = kd->closest_to_pt(p, maxdist2);
	if (!match)
		return false;
	int ind = (match - (const float *) &(mesh->vertices[0][0])) / 3;
	if (ind < 0 || ind >= mesh->vertices.size())
		return false;

	const vector<int> &a = mesh->adjacentfaces[ind];
	if (a.empty()) 
	{
		pmatch = mesh->vertices[ind];
		return true;
	}

	float closest_dist2 = BIGNUM;
	for (unsigned int i = 0; i < a.size(); i++) 
	{
		point c = closest_on_face(mesh, a[i], p);
		float this_dist2 = dist2(c, p);
		if (this_dist2 < closest_dist2) 
		{
			closest_dist2 = this_dist2;
			pmatch = c;
		}
	}
	
	return (closest_dist2 != BIGNUM);
}

// Color by distance to another mesh
void dist2mesh(TriMesh *mesh, TriMesh *othermesh, float maxdist)
{
	othermesh->need_adjacentfaces();
	othermesh->need_normals();
	KDtree *kd = new KDtree(othermesh->vertices);

	// This auto-scaling is useless for precise comparisons,
	// which is what you want this for anyways... -- SMR
	//float fs = feature_size(mesh);
	//float maxdist = atof(maxdist_) * fs;
	//TriMesh::dprintf("Using maxdist = %f\n", maxdist);
	float maxdist2 = sqr(maxdist);

	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		const point &p = mesh->vertices[i];
		point pmatch;
		float d = maxdist;
		if (find_closest_pt(othermesh, kd, p, maxdist2, pmatch))
			d = dist(p, pmatch);
		d /= maxdist;
		float H = 4.0f * (1.0f - d);
		float S = 0.7f + 0.3f * d;
		float V = 0.7f + 0.3f * d;
		mesh->colors[i] = Color::hsv(H,S,V);
	}
	delete kd;
}
//


void dist2mesh(TriMesh *mesh, TriMesh *othermesh, float maxdist,vector<float> &dists)
{
	othermesh->need_adjacentfaces();
	othermesh->need_normals();
	mesh->need_normals();

	KDtree *kd = new KDtree(othermesh->vertices);
	float maxdist2 = sqr(maxdist);

	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++) 
	{
		const point &p = mesh->vertices[i];
		point pmatch;
		float d = maxdist;
		if (find_closest_pt(othermesh, kd, p, maxdist2, pmatch))
		{
			d = dist(p, pmatch);
			//if(m_flag==1)d=-1*d;
		}
		dists.push_back(d);
	}

	//
	mesh->colors.clear();
	float maxdis=*(max_element(dists.begin(),dists.end()));
	float mindis=*(min_element(dists.begin(),dists.end()));
	float errMax=max(fabs(maxdis),fabs(mindis));
#pragma omp parallel for
	for ( int i=0; i<dists.size(); i++)
	{
		float d=dists[i];
		//float dd=fabs(d)/errMax;
		float R,G,B;
		if (d>=0 && d<0.125)
		{
			R = 0;
			G = 0.5*d/0.125;
			B = 1.0;
		}
		if(d>=0.125 && d<0.25)
		{
			R = 0;
			G = d/0.25;
			B = 1;
		}
		if (d>=0.25 && d< 0.5)
		{
			R = 0;
			G = 1;
			B = 2-4*d;
		}
		if (d>=0.5 && d< 0.75)
		{
			R = 4*d-2;
			G = 1;
			B = 0;
		}
		if (d>=0.75 )
		{
			R = 1;
			G = 4*(1-d);
			B = 0;
		}
		
		mesh->colors.push_back(Color(R,G,B));
	}
	//
	delete kd;
}
// Color by distance to given vertex
void findvert(TriMesh *mesh, const char *v_, const char *nedges_)
{
	int v = atoi(v_);
	int nedges = atoi(nedges_) + 1;
	int nv = mesh->vertices.size();
	mesh->need_neighbors();
	mesh->flags.resize(nv);
	for (int i = 0; i < nv; i++)
		mesh->flags[i] = (i == v) ? 0 : nedges;
	for (int iter = 1; iter < nedges; iter++) 
	{
		for (int i = 0; i < nv; i++) 
		{
			for (unsigned j = 0; j < mesh->neighbors[i].size(); j++) 
			{
				int n = mesh->neighbors[i][j];
				if (mesh->flags[n] + 1 < mesh->flags[i])
					mesh->flags[i] = mesh->flags[n] + 1;
			}
		}
	}
#pragma omp parallel for
	for (int i = 0; i < nv; i++)
	{
		float c = (float) mesh->flags[i] / nedges;
		mesh->colors[i] = Color(1.0f, c, c);
	}
}


// Adjust colors
void remapcolor(TriMesh *mesh, const char *scale_, const char *off_,	const char *gamma_)
{
	float scale = atof(scale_);
	float off = atof(off_);
	float gamma = 1.0f / atof(gamma_);
	int nv = mesh->vertices.size();
#pragma omp parallel for
	for (int i = 0; i < nv; i++)
	{
		Color &c = mesh->colors[i];
		c[0] = pow(c[0] * scale + off, gamma);
		c[1] = pow(c[1] * scale + off, gamma);
		c[2] = pow(c[2] * scale + off, gamma);
	}
}


// Color based on depth in direction (x,y,z)
// To find range, eliminates percentage p of points
void colorbydepth(TriMesh *mesh, const char *x_, const char *y_,	  const char *z_, const char *p_)
{
	vec dir(atof(x_), atof(y_), atof(z_));
	float p = atof(p_);

	int nv = mesh->vertices.size();
	vector<float> depths(nv);
#pragma omp parallel for
	for (int i = 0; i < nv; i++)
		depths[i] = mesh->vertices[i] DOT dir;

	float mind, maxd;
	if (p > 0.0f) 
	{
		int which = int(p * nv);
		nth_element(depths.begin(), depths.begin() + which, depths.end());
		mind = depths[which];

		which = nv - 1 - which;
		nth_element(depths.begin(), depths.begin() + which, depths.end());
		maxd = depths[which];
	} else 
	{
		mind = *min_element(depths.begin(), depths.end());
		maxd = *max_element(depths.begin(), depths.end());
	}
	float mult = 1.0f / (maxd - mind);
	for (int i = 0; i < nv; i++) 
	{
		float d = mesh->vertices[i] DOT dir;
		mesh->colors[i] = Color(mult * (d - mind));
	}
}

//红 		1.0  0.0  0.0
//橙 		1.0  0.5  0.0
//黄 		1.0  1.0  0.0
//			0.5  1.0  0.0      考虑新增这个颜色
//绿 		0.0  1.0  0.0
//			0.0  1.0  0.5      考虑新增这个颜色
//青 		0.0  1.0  1.0
//浅蓝		0.0  0.5  1.0
//蓝 		0.0  0.0  1.0
