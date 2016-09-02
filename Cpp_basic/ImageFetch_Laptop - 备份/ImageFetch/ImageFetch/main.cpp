// ImageFetch.cpp : 定义控制台应用程序的入口点。
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
		std::cout<<"Pidx is "<<Pidx<<std::endl;
		fflush(stdout);
	if(Pidx==CENTER)
	{
 		module=new ImageFetchGlobal(Pcnt,Pidx);
	}
	else
	{
		module=new ImageFetchLocal(Pcnt,Pidx,CENTER);
	}
	module->Run();
	delete module;
	MPI_Finalize();
	return 0;
}

