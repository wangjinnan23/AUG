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
	int            Pcnt;
	int            Pidx;
	MPI_Comm_size(MPI_COMM_WORLD, &Pcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &Pidx);
	ImageStitch imgStitch;
	//if (argc == 2 && strcmp(argv[1], "-g") == 0)
	//	imgStitch.generateStitchParams();
	//else
	//	imgStitch.run();
	if (Pidx == 0)
	{
		printf("Generation starts !");
		imgStitch.generateStitchParams();
		MPI_Barrier(MPI_COMM_WORLD);
		printf("Generation ends !");
	}
	else
	{
		MPI_Barrier(MPI_COMM_WORLD);	
	}
	imgStitch.run();
	MPI_Finalize();
	return(0);
}
