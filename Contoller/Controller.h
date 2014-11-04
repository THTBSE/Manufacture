#ifndef _FCL_CONTROLLER_H_
#define _FCL_CONTROLLER_H_

class vfclController
{
public:
	static enum proctype{NIL,VOLUMECALC,SETPLANE};
	vfclController():_procType(NIL){}
	void change_proc(int t){_procType = t;}
	int query_proc(){return _procType;}

private:
	int _procType;
};

#endif