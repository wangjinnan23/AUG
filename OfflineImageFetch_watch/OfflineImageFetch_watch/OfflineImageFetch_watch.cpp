#include "stdafx.h"
#include "ImageFetch.h"

#define CENTER 0
int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int Pcnt, Pidx;

	ImageFetch *module;
	MPI_Comm_size(MPI_COMM_WORLD, &Pcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &Pidx);

	if(Pidx == CENTER) 
	{
		module = new ImageFetchGlobal(Pcnt, Pidx);
	}
		
	else 
	{
		module = new ImageFetchLocal(Pcnt, Pidx, CENTER);
	}
	module->Run();

	delete module;
	MPI_Finalize();
	return(0);
}
//口点。

// hello.cpp : 定义控制台应用程序的入

//#include "stdafx.h"
//#include<opencv2/opencv.hpp>
//using namespace cv;
//int main(int argc, _TCHAR* argv[])
//{
//	Mat src = imread("E:\\preview.jpg");
//	imshow("src",src);
//	waitKey();
//	return 0;
//}

