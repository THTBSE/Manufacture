/*
diffuse.cpp
Smoothing of meshes and per-vertex fields
*/
#include <stdio.h>
#include "..\include\TriMesh.h"
#include "..\include\TriMesh_algo.h"
#include "..\include\timestamp.h"
#include <cmath>
using namespace std;


// Approximation to Gaussian...  Used in filtering
static inline float wt(const point &p1, const point &p2, float invsigma2)
{
	float d2 = invsigma2 * dist2(p1, p2);
	return (d2 >= 9.0f) ? 0.0f : exp(-0.5f*d2);
	//return (d2 >= 25.0f) ? 0.0f : exp(-0.5f*d2);
}
static inline float wt(const TriMesh *themesh, int v1, int v2, float invsigma2)
{
	return wt(themesh->vertices[v1], themesh->vertices[v2], invsigma2);
}
// Functor classes for adding scalar, vector, or tensor fields on the surface
template <class T>
struct AccumVec {
	const vector<T> &field;
	AccumVec(const vector<T> &field_) : field(field_)
		{}
	void operator() (const TriMesh *themesh, int v0, T &f,
			 float w, int v) const
	{
		f += w * field[v];
	}
};

struct AccumCurv 
{
	void operator() (const TriMesh *themesh, int v0, vec &c,
			 float w, int v) const
	{
		vec ncurv;
		proj_curv(themesh->pdir1[v], themesh->pdir2[v],
			  themesh->curv1[v], 0, themesh->curv2[v],
			  themesh->pdir1[v0], themesh->pdir2[v0],
			  ncurv[0], ncurv[1], ncurv[2]);
		c += w * ncurv;
	}
};

struct AccumDCurv
{
	void operator() (const TriMesh *themesh, int v0, Vec<4> &d,
			 float w, int v) const
	{
		Vec<4> ndcurv;
		proj_dcurv(themesh->pdir1[v], themesh->pdir2[v],
			   themesh->dcurv[v],
			   themesh->pdir1[v0], themesh->pdir2[v0],
			   ndcurv);
		d += w * ndcurv;
	}
};


// Diffuse a vector field at 1 vertex, weighted by
// a Gaussian of width 1/sqrt(invsigma2)
template <class ACCUM, class T>
static void diffuse_vert_field(TriMesh *themesh, const ACCUM &accum,
			       int v, float invsigma2, T &flt)
{
	if (themesh->neighbors[v].empty()) 
	{
		flt = T();
		accum(themesh, v, flt, 1.0f, v);
		return;
	}

	flt = T();
	accum(themesh, v, flt, themesh->pointareas[v], v);
	float sum_w = themesh->pointareas[v];
	const vec &nv = themesh->normals[v];

	unsigned &flag = themesh->flag_curr;
	flag++;
	themesh->flags[v] = flag;
	vector<int> boundary = themesh->neighbors[v];
	while (!boundary.empty()) {
		int n = boundary.back();
		boundary.pop_back();
		if (themesh->flags[n] == flag)
			continue;
		themesh->flags[n] = flag;
		if ((nv DOT themesh->normals[n]) <= 0.0f)
			continue;
		// Gaussian weight
		float w = wt(themesh, n, v, invsigma2);
		if (w == 0.0f)
			continue;
		// Downweight things pointing in different directions
		w *= nv DOT themesh->normals[n];
		// Surface area "belonging" to each point
		w *= themesh->pointareas[n];
		// Accumulate weight times field at neighbor
		accum(themesh, v, flt, w, n);
		sum_w += w;
		for (unsigned int i = 0; i < themesh->neighbors[n].size(); i++) {
			int nn = themesh->neighbors[n][i];
			if (themesh->flags[nn] == flag)
				continue;
			boundary.push_back(nn);
		}
	}
	flt /= sum_w;
}


// Smooth the mesh geometry.
// XXX - this is perhaps not a great way to do this,
// but it seems to work better than most other things I've tried...
void smooth_mesh(TriMesh *themesh, float sigma)
{
	themesh->need_faces();
	diffuse_normals(themesh, 0.5f * sigma);
	int nv = themesh->vertices.size();

	printf("\rSmoothing... ");
	timestamp t = now();

	float invsigma2 = 1.0f / sqr(sigma);

	vector<point> dflt(nv);
	for (int i = 0; i < nv; i++) {
		diffuse_vert_field(themesh, AccumVec<vec>(themesh->vertices),
				   i, invsigma2, dflt[i]);
		// Just keep the displacement
		dflt[i] -= themesh->vertices[i];
	}

	// Slightly better small-neighborhood approximation
	int nf = themesh->faces.size();
#pragma omp parallel for
	for (int i = 0; i < nf; i++) 
	{
		point c = themesh->vertices[themesh->faces[i][0]] +
			  themesh->vertices[themesh->faces[i][1]] +
			  themesh->vertices[themesh->faces[i][2]];
		c /= 3.0f;
		for (int j = 0; j < 3; j++) 
		{
			int v = themesh->faces[i][j];
			vec d = 0.5f * (c - themesh->vertices[v]);
			dflt[v] += themesh->cornerareas[i][j] /
				   themesh->pointareas[themesh->faces[i][j]] *
				   exp(-0.5f * invsigma2 * len2(d)) * d;
		}
	}

	// Filter displacement field
	vector<point> dflt2(nv);
	for (int i = 0; i < nv; i++) 
	{
		diffuse_vert_field(themesh, AccumVec<point>(dflt),	   i, invsigma2, dflt2[i]);
	}

	// Update vertex positions
#pragma omp parallel for
	for (int i = 0; i < nv; i++)
		themesh->vertices[i] += dflt[i] - dflt2[i]; // second Laplacian
}


// Filter a vertex using the method of [Jones et al. 2003]
// For pass 1, do simple smoothing and write to mpoints
// For pass 2, do bilateral, using mpoints, and write to themesh->vertices
static void jones_filter(TriMesh *themesh, int v,
	float invsigma2_1, float invsigma2_2, bool pass1,
	vector<point> &mpoints)
{
	const point &p = pass1 ? themesh->vertices[v] : mpoints[v];
	point &flt = pass1 ? mpoints[v] : themesh->vertices[v];

	flt = point();
	float sum_w = 0.0f;

	unsigned &flag = themesh->flag_curr;
	flag++;
	vector<int> boundary = themesh->adjacentfaces[v];
	while (!boundary.empty()) 
	{
		int f = boundary.back();
		boundary.pop_back();
		if (themesh->flags[f] == flag)
			continue;
		themesh->flags[f] = flag;

		int v0 = themesh->faces[f][0];
		int v1 = themesh->faces[f][1];
		int v2 = themesh->faces[f][2];
		const point &p0 = themesh->vertices[v0];
		const point &p1 = themesh->vertices[v1];
		const point &p2 = themesh->vertices[v2];
		point c = (p0 + p1 + p2) * (1.0f / 3.0f);

		float w = wt(p, c, invsigma2_1);
		if (w == 0.0f)
			continue;
		w *= len(trinorm(p0, p1, p2));

		if (pass1) 
		{
			flt += w * c;
			sum_w += w;
		} else {
			vec fn = trinorm(mpoints[v0], mpoints[v1], mpoints[v2]);
			normalize(fn);
			point prediction = p - fn * ((p - c) DOT fn);
			w *= wt(p, prediction, invsigma2_2);
			if (w == 0.0f)
				continue;
			flt += w * prediction;
			sum_w += w;
		}

		for (int i = 0; i < 3; i++)
		{
			int ae = themesh->across_edge[f][i];
			if (ae < 0 || themesh->flags[ae] == flag)
				continue;
			boundary.push_back(ae);
		}
	}
	if (sum_w == 0.0f)
		flt = p;
	else
		flt *= 1.0f / sum_w;
}


// Bilateral smoothing using the method of [Jones et al. 2003]
void bilateral_smooth_mesh(TriMesh *themesh, float sigma1, float sigma2)
{
	themesh->need_faces();
	themesh->need_adjacentfaces();
	themesh->need_across_edge();
	int nv = themesh->vertices.size(), nf = themesh->faces.size();
	if (themesh->flags.size() != nf)
		themesh->flags.resize(nf);

	printf("\rSmoothing... ");
	timestamp t = now();

	float sigma3 = 0.5f * sigma1;
	float invsigma2_1 = 1.0f / sqr(sigma1);
	float invsigma2_2 = 1.0f / sqr(sigma2);
	float invsigma2_3 = 1.0f / sqr(sigma3);

	// Pass I: mollification
	vector<point> mpoints(nv);
	for (int i = 0; i < nv; i++)
		jones_filter(themesh, i, invsigma2_3, 0.0f, true, mpoints);

	// Pass II: bilateral
	for (int i = 0; i < nv; i++)
		jones_filter(themesh, i, invsigma2_1, invsigma2_2, false, mpoints);

}


// Diffuse an arbitrary per-vertex vector field
template <class T>
void diffuse_vector(TriMesh *themesh, std::vector<T> &field, float sigma)
{
	themesh->need_normals();
	themesh->need_pointareas();
	themesh->need_neighbors();
	int nv = themesh->vertices.size();
	if (themesh->flags.size() != nv)
		themesh->flags.resize(nv);

	printf("\rSmoothing vector field... ");
	timestamp t = now();

	float invsigma2 = 1.0f / sqr(sigma);

	vector<T> flt(nv);
	AccumVec<T> a(field);
	for (int i = 0; i < nv; i++)
		diffuse_vert_field(themesh, a, i, invsigma2, flt[i]);

	field = flt;

	printf("Done.  Filtering took %f sec.\n", now() - t);
}


// Diffuse the normals across the mesh
void diffuse_normals(TriMesh *themesh, float sigma)
{
	themesh->need_normals();
	themesh->need_pointareas();
	themesh->need_neighbors();
	int nv = themesh->vertices.size();
	if (themesh->flags.size() != nv)
		themesh->flags.resize(nv);

	printf("\rSmoothing normals... ");
	timestamp t = now();

	float invsigma2 = 1.0f / sqr(sigma);

	vector<vec> nflt(nv);
	AccumVec<vec> a(themesh->normals);
	for (int i = 0; i < nv; i++) {
		diffuse_vert_field(themesh, a, i, invsigma2, nflt[i]);
		normalize(nflt[i]);
	}

	themesh->normals = nflt;

	printf("Done.  Filtering took %f sec.\n", now() - t);
}


// Diffuse the curvatures across the mesh
void diffuse_curv(TriMesh *themesh, float sigma)
{
	themesh->need_normals();
	themesh->need_pointareas();
	themesh->need_curvatures();
	themesh->need_neighbors();
	int nv = themesh->vertices.size();
	if (themesh->flags.size() != nv)
		themesh->flags.resize(nv);

	printf("\rSmoothing curvatures... ");
	timestamp t = now();

	float invsigma2 = 1.0f / sqr(sigma);

	vector<vec> cflt(nv);
	for (int i = 0; i < nv; i++)
		diffuse_vert_field(themesh, AccumCurv(), i, invsigma2, cflt[i]);
#pragma omp parallel for
	for (int i = 0; i < nv; i++)
		diagonalize_curv(themesh->pdir1[i], themesh->pdir2[i],
				 cflt[i][0], cflt[i][1], cflt[i][2],
				 themesh->normals[i],
				 themesh->pdir1[i], themesh->pdir2[i],
				 themesh->curv1[i], themesh->curv2[i]);

	printf("Done.  Filtering took %f sec.\n", now() - t);
}


// Diffuse the curvature derivatives across the mesh
void diffuse_dcurv(TriMesh *themesh, float sigma)
{
	themesh->need_normals();
	themesh->need_pointareas();
	themesh->need_curvatures();
	themesh->need_dcurv();
	themesh->need_neighbors();
	int nv = themesh->vertices.size();
	if (themesh->flags.size() != nv)
		themesh->flags.resize(nv);

	printf("\rSmoothing curvature derivatives... ");
	timestamp t = now();

	float invsigma2 = 1.0f / sqr(sigma);

	vector< Vec<4> > dflt(nv);
	for (int i = 0; i < nv; i++)
		diffuse_vert_field(themesh, AccumDCurv(), i, invsigma2, dflt[i]);

	themesh->dcurv = dflt;
	printf("Done.  Filtering took %f sec.\n", now() - t);
}


// Instantiate a bunch of diffuse_vector forms
template void diffuse_vector< float >(TriMesh *, vector< float > &, float);
template void diffuse_vector< Vec<2,float> >(TriMesh *, vector< Vec<2,float> > &, float);
template void diffuse_vector< Vec<3,float> >(TriMesh *, vector< Vec<3,float> > &, float);
template void diffuse_vector< Vec<4,float> >(TriMesh *, vector< Vec<4,float> > &, float);
