#include "stdafx.h"
#include "ImageStitch.h"


//#define _WINDOWS

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
