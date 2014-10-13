/*����
1. ��������Ͱ����ļ�
2. �������ݽṹ
3. ���ȿ���
�� ��Ļ�������
1. ƽ��������֮�����
2. �ж������Ƿ��غ�
3. ʸ�����
4. ʸ�����
5. �жϵ��Ƿ����߶���
6. ��һ����ĳ����ת�������
7. ��ʸ���н�
�� �߶μ�ֱ�ߵĻ�������
1. �����߶εĹ�ϵ
2. ��㵽�߶�����ֱ�ߴ��ߵĴ���
3. �㵽�߶ε������
4. �㵽�߶�����ֱ�ߵľ���
5. �㵽���߼����������
6. �ж�Բ�Ƿ��ڶ������
7. ��ʸ���н�����
8. ���߶�֮��ļн�
9. �ж��߶��Ƿ��ཻ
10.�ж��߶��Ƿ��ཻ�������ڶ˵㴦
11.������ĳֱ�ߵĶԳƵ�
12.�ж�����ֱ���Ƿ��ཻ����ֱ�߽���
13.�ж��߶��Ƿ��ཻ������ཻ���ؽ���
�� ����γ����㷨ģ��
1. �ж϶�����Ƿ�򵥶����
2. ������ζ����͹����
3. �ж϶�����Ƿ�͹�����
4. ���������
5. �ж϶���ζ�������з���
7. ���߷��жϵ��Ƿ��ڶ������
8. �жϵ��Ƿ���͹�������
9. Ѱ�ҵ㼯��graham�㷨
10.Ѱ�ҵ㼯͹���ľ������
11.͹��MelkMan�㷨��ʵ��
12. ͹����ε�ֱ��
13.��͹����ε�����
===========================================================================*/

//����
/* ��Ҫ������ͷ�ļ� */
#pragma   once
#include <cmath>
namespace BaseClass{
namespace Geometry2D
{
	/* �������� */
	const double INF = 1E20;
	const double EP = 1E-10;
	const int MAXV = 300;
	 const double PI0 = 3.14159265;
	/* �������νṹ */
	struct POINT
	{
		double x;
		double y;
		POINT(double a=0, double b=0) { x=a; y=b;}
	};
	struct LINESEG
	{
		POINT s;
		POINT e;
		LINESEG(POINT a, POINT b) { s=a; e=b;}
		LINESEG() { }
	};
	// ֱ�ߵĽ������� a*x+b*y+c=0 Ϊͳһ��ʾ��Լ�� a>= 0
	struct LINE
	{
		double a;
		double b;
		double c;
		LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}
		LINE(POINT p0, POINT p1) 
		{   
			POINT np(p1.x-p0.x,p1.y-p0.y);
			a=np.y;
		    b=-np.x;
			c=np.x*p0.y-np.y*p0.x;
		    if (a<0)
		    {   
				a=-a;
				b=-b;
				c=-c;
		    }
		}
	};
	//�������Ĵ���
	inline int dblcmp(double d)
	{
		if(fabs(d)<EP)
			return 0 ;
		return (d>0) ?1 :-1 ;
	}
	//<һ>��Ļ�������
		// ��������֮��ŷ�Ͼ���
	inline	double dist(POINT p1,POINT p2)
	{
		return( sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) ) );
	}
	// �ж��������Ƿ��غ�
	inline bool equal_point(POINT p1,POINT p2)
	{
		return ( (abs(p1.x-p2.x)<EP)&&(abs(p1.y-p2.y)<EP) );
	}
	/*(sp-op)*(ep-op)�Ĳ��
	r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)�Ĳ��
	r>0:sp��ʸ��op ep��˳ʱ�뷽��
	r=0��op sp ep���㹲�ߣ�
	r<0: sp��ʸ��op ep����ʱ�뷽�� */
	inline double multiply(POINT sp,POINT ep,POINT op)
	{
		return((sp.x-op.x)*(ep.y-op.y) - (ep.x-op.x)*(sp.y-op.y));
	}
	inline double amultiply(POINT sp,POINT ep,POINT op)
	{
		return fabs((sp.x-op.x)*(ep.y-op.y)-(ep.x-op.x)*(sp.y-op.y));
	}
	/*ʸ��(p1-op)��(p2-op)�ĵ��
	r=dotmultiply(p1,p2,op),�õ�ʸ��(p1-op)��(p2-op)�ĵ���������ʸ��������ʸ��
	r < 0: ��ʸ���н�Ϊ��ǣ�
	r = 0����ʸ���н�Ϊֱ�ǣ�
	r > 0: ��ʸ���н�Ϊ�۽� */
	inline double dotmultiply(POINT p1,POINT p2,POINT p0)
	{
		return ((p1.x-p0.x)*(p2.x-p0.x) + (p1.y-p0.y)*(p2.y-p0.y));
	}
	/* �жϵ�p�Ƿ����߶�l��
	������(p���߶�l���ڵ�ֱ����)&& (��p�����߶�lΪ�Խ��ߵľ�����) */
	inline bool online(LINESEG l,POINT p)
	{
		return ((multiply(l.e,p,l.s)==0)
			&& ( ( (p.x-l.s.x) * (p.x-l.e.x) <=0 ) && ( (p.y-l.s.y)*(p.y-l.e.y) <=0 ) ) );
	}
	// ���ص�p�Ե�oΪԲ����ʱ����תalpha(��λ������)�����ڵ�λ��
	inline POINT rotate(POINT o,double alpha,POINT p)
	{
		POINT tp;
		p.x -=o.x;
		p.y -=o.y;
		tp.x=p.x*cos(alpha) - p.y*sin(alpha)+o.x;
		tp.y=p.y*cos(alpha) + p.x*sin(alpha)+o.y;
		return tp;
	}
	/* ���ض�����o�㣬��ʼ��Ϊos����ֹ��Ϊoe�ļн�(��λ������)
	�Ƕ�С��pi��������ֵ
	�Ƕȴ���pi�����ظ�ֵ
	�����������߶�֮��ļн� */
	inline double angle(POINT o,POINT s,POINT e)
	{
		double cosfi,fi,norm;
		double dsx = s.x - o.x;
		double dsy = s.y - o.y;
		double dex = e.x - o.x;
		double dey = e.y - o.y;
		cosfi=dsx*dex+dsy*dey;
		norm=(dsx*dsx+dey*dey)*(dex*dex+dey*dey);
		cosfi /= sqrt( norm );
		if (cosfi >= 1.0 ) return 0;
		if (cosfi <= -1.0 ) return -3.1415926;
		fi=acos(cosfi);
		if (dsx*dey-dsy*dex>0) return fi;// ˵��ʸ��os ��ʸ�� oe��˳ʱ�뷽��
		return -fi;
	}
	//<��>�߶μ�ֱ�ߵĻ�������
		/* �жϵ�C���߶�AB���ڵ�ֱ��l�ϴ���P�����߶�AB�Ĺ�ϵ
		�������Ǹ�������Ĺ�ʽд�ģ�P�ǵ�C���߶�AB����ֱ�ߵĴ���
		AC dot AB
		r = ----------------------
		||AB||^2
		(Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
		= ----------------------------------------------------
		L^2
		r has the following meaning:
		r=0 P = A
		r=1 P = B
		r<0 P is on the backward extension of AB
		r>1 P is on the forward extension of AB
		0<r<1 P is interior to AB
		*/
	inline	double relation(POINT c,LINESEG l)
	{
		LINESEG tl;
		tl.s=l.s;
		tl.e=c;
		return dotmultiply(tl.e,l.e,l.s)/(dist(l.s,l.e)*dist(l.s,l.e));
	}
	// ���C���߶�AB����ֱ�ߵĴ��� P
	inline POINT perpendicular(POINT p,LINESEG l)
	{
		double r=relation(p,l);
		POINT tp;
		tp.x=l.s.x+r*(l.e.x-l.s.x);
		tp.y=l.s.y+r*(l.e.y-l.s.y);
		return tp;
	}
	/* ���p���߶�l����̾���
	�����߶��Ͼ�õ�����ĵ�np ע�⣺np���߶�l�ϵ���p����ĵ㣬��һ���Ǵ��� */
	inline double ptolinesegdist(POINT p,LINESEG l,POINT &np)
	{
		double r=relation(p,l);
		if(r<0)
		{
			np=l.s;
			return dist(p,l.s);
		}
		if(r>1)
		{
			np=l.e;
			return dist(p,l.e);
		}
		np=perpendicular(p,l);
		return dist(p,np);
	}
	// ���p���߶�l����ֱ�ߵľ���
	//��ע�Ȿ�������ϸ�����������
	inline double ptoldist(POINT p,LINESEG l)
	{
		return abs(multiply(p,l.e,l.s))/dist(l.s,l.e);
	}
	/* ����㵽���߼����������,�����������.
	ע�⣺���õ���ptolineseg()���� */
	inline double ptopointset(int vcount, POINT pointset[], POINT p, POINT &q)
	{
		int i;
		double cd=double(INF),td;
		LINESEG l;
		POINT tq,cq;
		for(i=0;i<vcount-1;i++)
		{
			l.s=pointset[i];
			l.e=pointset[i+1];
			td=ptolinesegdist(p,l,tq);
			if(td<cd)
			{
				cd=td;
				cq=tq;
			}
		}
		q=cq;
		return cd;
	}
	/* �ж�Բ�Ƿ��ڶ������*/
	inline bool CircleInsidePolygon(int vcount,POINT center,double radius,POINT polygon[])
	{
		POINT q;
		double d;
		q.x=0;
		q.y=0;
		d=ptopointset(vcount,polygon,center,q);
		if(d<radius||fabs(d-radius)<EP) return true;
		else return false;
	}
	/* ��������ʸ��l1��l2�ļнǵ����� (-1 ~ 1)
	ע�⣺������������нǵĻ���ע�ⷴ���Һ�����ֵ���Ǵ� 0��pi */
	 inline double cosine(LINESEG l1,LINESEG l2)
	{
		return(((l1.e.x-l1.s.x)*(l2.e.x-l2.s.x)+(l1.e.y-l1.s.y)*(l2.e.y-l2.s.y))/(dist(l1.e,l1.s)*dist(l2.e,l2.s))) ;
	}
	// �����߶�l1��l2֮��ļн�
	//��λ�����ȷ�Χ(-pi��pi)
	inline double lsangle(LINESEG l1,LINESEG l2)
	{
		POINT o,s,e;
		o.x=o.y=0;
		s.x=l1.e.x-l1.s.x;
		s.y=l1.e.y-l1.s.y;
		e.x=l2.e.x-l2.s.x;
		e.y=l2.e.y-l2.s.y;
		return angle(o,s,e);
	}
	//�ж��߶�u��v�ཻ(�����ཻ�ڶ˵㴦)
	inline bool intersect(LINESEG u,LINESEG v)
	{
		return ( (max(u.s.x,u.e.x)>=min(v.s.x,v.e.x))&& //�ų�ʵ��
			(max(v.s.x,v.e.x)>=min(u.s.x,u.e.x))&&
			(max(u.s.y,u.e.y)>=min(v.s.y,v.e.y))&&
			(max(v.s.y,v.e.y)>=min(u.s.y,u.e.y))&&
			(multiply(v.s,u.e,u.s)*multiply(u.e,v.e,u.s)>=0)&& //����ʵ��
			(multiply(u.s,v.e,v.s)*multiply(v.e,u.e,v.s)>=0));
	}
	// �ж��߶�u��v�ཻ��������˫���Ķ˵㣩
	inline bool intersect_A(LINESEG u,LINESEG v)
	{
		return ((intersect(u,v)) &&
			(!online(u,v.s)) &&
			(!online(u,v.e)) &&
			(!online(v,u.e)) &&
			(!online(v,u.s)));
	}
	// �ж��߶�v����ֱ�����߶�u�ཻ
	//�������ж��߶�u�Ƿ�����߶�v
	inline	bool intersect_l(LINESEG u,LINESEG v)
	{
		return multiply(u.s,v.e,v.s)*multiply(v.e,u.e,v.s)>=0;
	}
	// ������֪�������꣬����������ֱ�߽������̣� a*x+b*y+c = 0 (a >= 0)
	inline LINE makeline(POINT p1,POINT p2)
	{
		LINE tl;
		int sign = 1;
		tl.a=p2.y-p1.y;
		if(tl.a<0)
		{
			sign = -1;
			tl.a=sign*tl.a;
		}
		tl.b=sign*(p1.x-p2.x);
		tl.c=sign*(p1.y*p2.x-p1.x*p2.y);
		return tl;
	}
	// ����ֱ�߽������̷���ֱ�ߵ�б��k,ˮƽ�߷��� 0,��ֱ�߷��� 1e200
	inline double slope(LINE l)
	{
		if(abs(l.a) < 1e-20)return 0;
		if(abs(l.b) < 1e-20)return INF;
		return -(l.a/l.b);
	}
	// ����ֱ�ߵ���б��alpha ( 0 - pi)
	// ע�⣺atan()���ص��� -PI/2 ~ PI/2
	inline double alpha(LINE l)
	{
		if(abs(l.a)< EP)return 0;
		if(abs(l.b)< EP)return PI0/2;
		double k=slope(l);
		if(k>0)
			return atan(k);
		else
			return PI0+atan(k);
	}
	// ���p����ֱ��l�ĶԳƵ�
	inline POINT symmetry(LINE l,POINT p)
	{
		POINT tp;
		tp.x=((l.b*l.b-l.a*l.a)*p.x-2*l.a*l.b*p.y-2*l.a*l.c)/(l.a*l.a+l.b*l.b);
		tp.y=((l.a*l.a-l.b*l.b)*p.y-2*l.a*l.b*p.x-2*l.b*l.c)/(l.a*l.a+l.b*l.b);
		return tp;
	}
	// �������ֱ�� l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)�ཻ������true���ҷ��ؽ���p
	inline bool lineintersect(LINE l1,LINE l2,POINT &p) // �� L1��L2
	{
		double d=l1.a*l2.b-l2.a*l1.b;
		if(abs(d)<EP) // ���ཻ
			return false;
		p.x = (l2.c*l1.b-l1.c*l2.b)/d;
		p.y = (l2.a*l1.c-l1.a*l2.c)/d;
		return true;
	}
	// ����߶�l1��l2�ཻ������true�ҽ�����(inter)���أ����򷵻�false
	inline bool intersection(LINESEG l1,LINESEG l2,POINT &inter)
	{
		LINE ll1,ll2;
		ll1=makeline(l1.s,l1.e);
		ll2=makeline(l2.s,l2.e);
		if(lineintersect(ll1,ll2,inter)) return online(l1,inter);
		else return false;
	}
	//<��> ����γ����㷨ģ��
		//������ر�˵�����������ζ���Ҫ����ʱ������
		// ���ض�������(signed)��
		// ���붥�㰴��ʱ������ʱ��������ֵ�����򷵻ظ�ֵ
	inline	double area_of_polygon(int vcount,POINT polygon[])
	{
		int i;
		double s;
		if (vcount<3)
			return 0;
		s=polygon[0].y*(polygon[vcount-1].x-polygon[1].x);
		for (i=1;i<vcount;i++)
			s+=polygon[i].y*(polygon[(i-1)].x-polygon[(i+1)%vcount].x);
		return s/2;
	}
	// �ж϶����Ƿ���ʱ������
	// ������붥�㰴��ʱ�����У�����true
   inline bool isconterclock(int vcount,POINT polygon[])
	{
		return area_of_polygon(vcount,polygon)>0;
	}
	/*���߷��жϵ�q������polygon��λ�ù�ϵ
	Ҫ��polygonΪ�򵥶���Σ�����ʱ������
	������ڶ�����ڣ� ����0
	������ڶ���α��ϣ�����1
	������ڶ�����⣺ ����2 */
	inline int insidepolygon(POINT q,POINT Polygon[],int vcount)
	{
		int c=0,i,n;
		LINESEG l1,l2;
		l1.s=q; l1.e=q;l1.e.x=double(INF);
		n=vcount;
		for (i=0;i<vcount;i++)
		{
			l2.s=Polygon[i];
			l2.e=Polygon[(i+1)%vcount];
			double ee= Polygon[(i+2)%vcount].x;
			double ss= Polygon[(i+3)%vcount].y;
			if(online(l2,q))
				return 1;
			if(intersect_A(l1,l2))
				c++; // �ཻ�Ҳ��ڶ˵�
			if(online(l1,l2.e)&& !online(l1,l2.s) && l2.e.y>l2.e.y)
				c++;//l2��һ���˵���l1���Ҹö˵������˵���������ϴ���Ǹ�
			if(!online(l1,l2.e)&& online(l1,l2.s) && l2.e.y<l2.e.y)
				c++;//����ƽ�б�
		}
		if(c%2 == 1)
			return 0;
		else
			return 2;
	}
	//�жϵ�q��͹�����polygon��
	// ��q��͹�����polygon��[��������]ʱ������true
	// ע�⣺�����polygonһ��Ҫ��͹�����
	inline bool InsideConvexPolygon(int vcount,POINT polygon[],POINT q)
	{
		POINT p;
		LINESEG l;
		int i;
		p.x=0; p.y=0;
		for(i=0;i<vcount;i++) // Ѱ��һ���϶��ڶ����polygon�ڵĵ�p������ζ���ƽ��ֵ
		{
			p.x+=polygon[i].x;
			p.y+=polygon[i].y;
		}
		p.x /= vcount;
		p.y /= vcount;
		for(i=0;i<vcount;i++)
		{
			l.s=polygon[i];
			l.e=polygon[(i+1)%vcount];
			if(multiply(p,l.e,l.s)*multiply(q,l.e,l.s)<0)
				/* ��p�͵�q�ڱ�l�����࣬˵����q�϶��ڶ������ */
				return false;
		}
		return true;
	}
	/*Ѱ��͹����graham ɨ�跨
	PointSetΪ����ĵ㼯��
	chΪ�����͹���ϵĵ㼯��������ʱ�뷽������;
	nΪPointSet�еĵ����Ŀ
	lenΪ�����͹���ϵĵ�ĸ��� */
	inline void Graham_scan(POINT PointSet[],POINT ch[],int n,int &len)
	{
		int i,j,k=0,top=2;
		POINT tmp;
		// ѡȡPointSet��y������С�ĵ�PointSet[k]����������ĵ��ж������ȡ����ߵ�һ��
		for(i=1;i<n;i++)
			if ( PointSet[i].y<PointSet[k].y || (PointSet[i].y==PointSet[k].y)
				&& (PointSet[i].x<PointSet[k].x) )
				k=i;
		tmp=PointSet[0];
		PointSet[0]=PointSet[k];
		PointSet[k]=tmp; // ����PointSet��y������С�ĵ���PointSet[0]
		for (i=1;i<n-1;i++) /* �Զ��㰴�����PointSet[0]�ļ��Ǵ�С����������򣬼�����ͬ
							�İ��վ���PointSet[0]�ӽ���Զ�������� */
		{
			k=i;
			for (j=i+1;j<n;j++)
				if ( multiply(PointSet[j],PointSet[k],PointSet[0])>0 || // ���Ǹ�С
					(multiply(PointSet[j],PointSet[k],PointSet[0])==0) && /*������ȣ�������� */ dist(PointSet[0],PointSet[j])<dist(PointSet[0],PointSet[k]) )
					k=j;
			tmp=PointSet[i];
			PointSet[i]=PointSet[k];
			PointSet[k]=tmp;
		}
		ch[0]=PointSet[0];
		ch[1]=PointSet[1];
		ch[2]=PointSet[2];
		for (i=3;i<n;i++)
		{
			while (multiply(PointSet[i],ch[top],ch[top-1])>=0) top--;
			ch[++top]=PointSet[i];
		}
		len=top+1;
	}
	// ���������㼯͹�ǣ�����˵��ͬgraham�㷨
	inline void ConvexClosure(POINT PointSet[],POINT ch[],int n,int &len)
	{
		int top=0,i,index,first;
		double curmax,curcos,curdis;
		POINT tmp;
		LINESEG l1,l2;
		bool use[MAXV];
		tmp=PointSet[0];
		index=0;
		// ѡȡy��С�㣬�������һ������ѡȡ�����
		for(i=1;i<n;i++)
		{
			if(PointSet[i].y<tmp.y||PointSet[i].y == tmp.y&&PointSet[i].x<tmp.x)
			{
				index=i;
			}
			use[i]=false;
		}
		tmp=PointSet[index];
		first=index;
		use[index]=true;
		index=-1;
		ch[top++]=tmp;
		tmp.x-=100;
		l1.s=tmp;
		l1.e=ch[0];
		l2.s=ch[0];
		while(index!=first)
		{
			curmax=-100;
			curdis=0;
			// ѡȡ�����һ��ȷ���߼н���С�ĵ㣬������ֵ�����
			for(i=0;i<n;i++)
			{
				if(use[i])continue;
				l2.e=PointSet[i];
				curcos=cosine(l1,l2); // ����cosֵ��н����ң���Χ�ڣ�-1 -- 1 ��
				if(curcos>curmax || fabs(curcos-curmax)<1e-6 && dist(l2.s,l2.e)>curdis)
				{
					curmax=curcos;
					index=i;
					curdis=dist(l2.s,l2.e);
				}
			}
			use[first]=false; //��յ�first�������־��ʹ������γɷ�յ�hull
			use[index]=true;
			ch[top++]=PointSet[index];
			l1.s=ch[top-2];
			l1.e=ch[top-1];
			l2.s=ch[top-1];
		}
		len=top-1;
	}
	// ��͹����ε�����,Ҫ���������ΰ���ʱ������
	inline POINT gravitycenter(int vcount,POINT polygon[])
	{
		POINT tp;
		double x,y,s,x0,y0,cs,k;
		x=0;y=0;s=0;
		for(int i=1;i<vcount-1;i++)
		{
			x0=(polygon[0].x+polygon[i].x+polygon[i+1].x)/3;
			y0=(polygon[0].y+polygon[i].y+polygon[i+1].y)/3; //��ǰ�����ε�����
			cs=multiply(polygon[i],polygon[i+1],polygon[0])/2;
			//�������������ֱ�����øù�ʽ���
			if(abs(s)<1e-20)
			{
				x=x0;y=y0;s+=cs;continue;
			}
			k=cs/s; //���������
			x=(x+k*x0)/(1+k);
			y=(y+k*y0)/(1+k);
			s += cs;
		}
		tp.x=x;
		tp.y=y;
		return tp;
	}
	/*��ν͹����ε�ֱ������͹���������������������롣������㷨
	����ʱO(n)����һ��������㷨�����������һ��͹����Σ��Ҷ���
	���밴˳��˳ʱ�롢��ʱ����ɣ��������롣�����벻��͹�����
	����һ��㼯����Ҫ������͹���� ��������������Ŷԣ�Ȼ�����ÿ
	���ŶԵľ��룬ȡ����ߡ�����Ҫ����5��*/
	inline void Diameter(POINT ch[],int n,double &dia)
	{
		int znum=0,i,j,k=1;
		int zd[MAXV][2];
		double tmp;
		while(amultiply(ch[0],ch[k+1],ch[n-1]) > amultiply(ch[0],ch[k],ch[n-1])-EP)
			k++;
		i=0;
		j=k;
		while(i<=k && j<n)
		{
			zd[znum][0]=i;
			zd[znum++][1]=j;
			while(amultiply(ch[i+1],ch[j+1],ch[i]) > amultiply(ch[i+1],ch[j],ch[i])-EP
				&& j< n-1)
			{
				zd[znum][0]=i;
				zd[znum++][1]=j;
				j++;
			}
			i++;
		}
		dia=-1.0;
		for(i=0;i<znum;i++)
		{
			printf("%d %d\n",zd[i][0],zd[i][1]);
			tmp=dist(ch[zd[i][0]],ch[zd[i][1]]);
			if(dia<tmp)
				dia=tmp;
		}
	}

}
}