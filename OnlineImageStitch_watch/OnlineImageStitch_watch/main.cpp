// ImageStitchOnline.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ImageStitch.h"

//#ifdef _WINDOWS
//const string FILENAME_PARAMS =(".\\ImgStitch.config");
//#else
//const string FILENAME_PARAMS =("./ImgStitch.config");
//#endif
int main(int argc, char* argv[]) {
	MPI_Init(&argc,&argv);
	ImageStitch imgStitch;
	if (argc == 2 && strcmp(argv[1], "-g") == 0)
		imgStitch.generateStitchParams();
	else
		imgStitch.run();
	MPI_Finalize();
	return(0);
}