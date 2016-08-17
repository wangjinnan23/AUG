// ImageFetch.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include"ImageFetch.h"
#define CENTER 0

int main(int argc, char* argv[])
{
	MPI_Init(&argc,&argv);
	int Pcnt,Pidx;
	ImageFetch *module;
	MPI_Comm_size(MPI_COMM_WORLD,&Pcnt);
	MPI_Comm_rank(MPI_COMM_WORLD,&Pidx);
	if(Pidx==CENTER)
	{
 		module=new ImageFetchGlobal(Pcnt,Pidx);
	}
	else
	{
	
	}
	module->Run();
	delete module;
	MPI_Finalize();
	return 0;
}

