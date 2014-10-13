/*
TriMesh_stats.cpp
Computation of various statistics on the mesh.
*/

#include "..\include\TriMesh.h"
#include "..\include\KDtree.h"
#include <algorithm>
#include <numeric>
using namespace std;


// A characteristic "feature size" for the mesh.  Computed as an approximation
// to the median edge length
float TriMesh::feature_size()
{
	need_faces();
	if (faces.empty())
		return 0.0f;

	int nf = faces.size();
	int nsamp = min(nf / 2, 333);

	vector<float> samples;
	samples.reserve(nsamp * 3);

	for (int i = 0; i < nsamp; i++) 
	{
		// Quick 'n dirty portable random number generator
		static unsigned randq = 0;
		randq = unsigned(1664525) * randq + unsigned(1013904223);

		int ind = randq % nf;
		const point &p0 = vertices[faces[ind][0]];
		const point &p1 = vertices[faces[ind][1]];
		const point &p2 = vertices[faces[ind][2]];
		samples.push_back(dist2(p0,p1));
		samples.push_back(dist2(p1,p2));
		samples.push_back(dist2(p2,p0));
	}
	float lsum = std::accumulate(samples.begin(),samples.end(),static_cast<float>(0.0));
// 	nth_element(samples.begin(),    samples.begin() + samples.size()/2,    samples.end());
// 	return sqrt(samples[samples.size()/2]);
	return sqrt(lsum/static_cast<int>(samples.size()));
}

float TriMesh::need_rou()
{
	int vn=vertices.size();
	KDtree kd(vertices);
	vector<float> l;
	for (int i=0; i<vn; i=i+5)
	{
		vector<const float *> knn;
		kd.find_k_closest_to_pt(knn, 5, vertices[i]);
		int n=knn.size();
		vector<float> li;
		for (int j=1;j<n;j++)
		{
			float d=dist2(vertices[i],vec(knn[j]));
			li.push_back(d);
		}
		sort(li.begin(),li.end());
		l.push_back(li[0]);
	}
	sort(l.begin(),l.end());
	float r=sqrtf(l[l.size()/2]);
	return r;
}

float mesh_volume(TriMesh* mesh)
{
	assert(mesh);
	const vector<TriMesh::Face>& f = mesh->faces;
	float volume = 0.0f;
	for_each(f.begin(),f.end(),[&volume,mesh](const TriMesh::Face& ftri)
	{
		const point& vi = mesh->vertices[ftri[0]];
		const point& vj = mesh->vertices[ftri[1]];
		const point& vk = mesh->vertices[ftri[2]];

		vec nor = (vj - vi) % (vk - vi);
		volume += (nor)^(vi + vj + vk);
	});

	volume /= 18.0f;
	return volume;
}