#include "extractFeatureEdges.h"
#include "../opengl/glew.h"
#include "../BaseToolClass/FCLToolkit.h"

void FeatureEdges::extract_feature_edges()
{
	objMesh->need_across_edge();
	int fsize = static_cast<int>(objMesh->faces.size());
	set<Pair>::iterator pIter;
	for (int i=0; i<fsize; i++)
	{
		for (int j=0; j<3; j++)
		{
			float diheralAng = objMesh->dihedral_m(i,j);
			if (diheralAng > diheralThr)
			{
				int v1 = std::min(objMesh->faces[i][(j+1)%3],objMesh->faces[i][(j+2)%3]);
				int v2 = std::max(objMesh->faces[i][(j+1)%3],objMesh->faces[i][(j+2)%3]);
				if (!is_in_standard(objMesh->vertices[v1]) || 
					!is_in_standard(objMesh->vertices[v2]))
					continue;
				Pair fedge(v1,v2);
				pIter = fEdges.find(fedge);
				if (pIter == fEdges.end())
				{
					fEdges.insert(fedge);
				}
			}
		}
	}
	
	objMesh->need_neighbors();
	vector<int> tempEdges;
	while (!fEdges.empty())
	{
		pIter = fEdges.begin();
		int pbeg = pIter->first;
		if (circle_finder(pIter, tempEdges))
		{
			featureCircle.push_back(tempEdges);
		}
		else
			featureCircle.push_back(tempEdges);
		/*
		else
		{
		EdgeIter pFinder;
		int numberOfNextEdges(0);
		numberOfNextEdges = std::count_if(objMesh->neighbors[pbeg].begin(),
		objMesh->neighbors[pbeg].end(), [this,&pbeg,&pIter,&pFinder](int pjoint)
		{
		pFinder = fEdges.find(Pair(std::min(pbeg, pjoint), std::max(pbeg, pjoint)));
		if (pFinder != fEdges.end())
		{
		pIter = pFinder;
		return true;
		}
		else
		return false;
		});

		if (numberOfNextEdges == 1)
		{
		vector<int> otherDirEdges;
		bool nil = circle_finder(pIter, otherDirEdges);
		std::reverse(otherDirEdges.begin(), otherDirEdges.end());
		otherDirEdges.pop_back();
		featureCircle.push_back(otherDirEdges);
		featureCircle.back().insert(featureCircle.back().end(), tempEdges.begin(),
		tempEdges.end());
		}
		}
		*/
		tempEdges.clear();
	}
}

bool FeatureEdges::circle_finder(EdgeIter ei, vector<int>& pts)
{
	int pbeg,pprev;
	pbeg = pprev = ei->first;
	int pjoint = ei->second;
	fEdges.erase(ei);
	pts.push_back(pbeg);
	pts.push_back(pjoint);
	EdgeIter pFinder;
	while (!objMesh->neighbors[pjoint].empty())
	{
		bool found = false;
		for (size_t i=0; i<objMesh->neighbors[pjoint].size(); i++)
		{
			int pnext = objMesh->neighbors[pjoint][i];
			if (pnext == pprev)
				continue;
			pFinder = fEdges.find(Pair(std::min(pjoint,pnext),std::max(pjoint,pnext)));
			if (pFinder != fEdges.end())
			{
				if (pnext == pbeg)
				{
					fEdges.erase(pFinder);
					return true;
				}
				else
				{
					found = true;
					pts.push_back(pnext);
					pprev = pjoint;
					pjoint = pnext;
					fEdges.erase(pFinder);
					break;
				}
			}
		}
		if (!found)
		{
//			pts.clear();
			return false;
		}
	}
	return false;
}

vector<int>& FeatureEdges::output_borderline(int k)
{
	assert(k<featureCircle.size());
	return featureCircle[k];
}

void FeatureEdges::draw_borderline()
{
	if (featureCircle.empty())
		return;

	glEnable(GL_COLOR_MATERIAL);
	for (size_t i=0; i<featureCircle.size(); i++)
	{
		if (highlightC == i)
			glColor3f(1.0f,0.0f,0.0f);
		else
			glColor3f(1.0f,1.0f,0.0f);
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		for (size_t j=0; j<featureCircle[i].size(); j++)
		{
			glVertex3fv(&objMesh->vertices[featureCircle[i][j]][0]);
		}
		glEnd();
	}
	glDisable(GL_COLOR_MATERIAL);
}

//k is the point index 
bool FeatureEdges::set_highlight(int k)
{
	for (size_t i=0; i<featureCircle.size(); i++)
	{
		vector<int>::iterator it;
		it = find(featureCircle[i].begin(),featureCircle[i].end(),k);
		if (it != featureCircle[i].end())
		{
			highlightC = i;
			return true;
		}
	}
	return false;
}

int FeatureEdges::get_highlight(vector<int>& hl)
{
	if (highlightC < 0 || highlightC >= static_cast<int>(featureCircle.size()))
		return -1;

	hl.clear();
	hl.assign(featureCircle[highlightC].begin(),featureCircle[highlightC].end());
	return highlightC;
}

void FeatureEdges::get_standard_curve(const TriMesh* mesh)
{
	vfclToolkit::projectToXZPlane(standardCurve, mesh);
}

bool FeatureEdges::is_in_standard(const point& pt)
{
	//if non standardCurve , keep all.
	if (standardCurve.empty())
		return true;
	vector<point>::iterator iter;
	iter = std::lower_bound(standardCurve.begin(), standardCurve.end(), pt, []
		(const point& a, const point& b){return a[0] < b[0]; });

	if (iter == standardCurve.end())
		return false;
	const point& pp = *iter;
	if (iter == standardCurve.begin())
	{
		if (pp[0] - pt[0] > 0.2)
			return false;
	}
	
	/*
	const point& prev = *(iter - 1);
	const point& curr = *(iter);
	point zlower = std::min(curr, prev, [](const point& a, const point& b)
	{return a[2] < b[2]; });
	point zupper = std::max(curr, prev, [](const point& a, const point& b)
	{return a[2] < b[2]; });
	
	if (zlower[2] < pt[2] && pt[2] < zupper[2])
		return true;
	else
		return false;
	*/
	
//	const point& pp = *(iter);
	float d = fabs(pp[2] - pt[2]);
	if (d < 0.2f)
		return true;
	else
		return false;
}

void FeatureEdges::draw_project_curve()
{
	if (standardCurve.empty())
		return;
	
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_LINE_STRIP);
	glLineWidth(5.0f);
	std::for_each(standardCurve.begin(), standardCurve.end(), [](const point& v)
	{
		glVertex3fv(&v[0]);
	});
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
}