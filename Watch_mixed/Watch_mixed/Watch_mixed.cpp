#include "ImageStitch.h"
#include "stdafx.h"
#include "ImageFetch.h"
#define CENTER 0
#ifdef _WINDOWS
const string FILENAME_PARAMS =(".\\ImgStitch.config");
#else
const string FILENAME_PARAMS =("./ImgStitch.config");
#endif
int main(int argc, char* argv[]) {
	MPI_Init(&argc,&argv);
	int Pcnt, Pidx;
	MPI_Comm_size(MPI_COMM_WORLD, &Pcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &Pidx);

 //cout<<"argc = "<<argc<<endl;  
 //   cout<<"Command line arguments received are:"<<endl;  
 //   for (int i=0;i<argc;i++)  
 //   {  
 //       cout<<"argument "<<i+1<<": "<<argv[i]<<endl;  
 //   }  
	if(Pcnt==1&& strcmp(argv[1], "-g") == 0)
	{
		printf("argc is %d . Pcnt=%d \n GenerateStitchParams !!\n",argc,Pcnt);
		ImageStitch imgStitch;  //在线拼接
		imgStitch.generateStitchParams();
	}
	else if(Pcnt==36)
	{
		if(Pidx==0)
		{
			printf("Choose the online watch !!\n");
		}
		ImageStitch imgStitch;  //在线观看
		if (argc == 2 && strcmp(argv[1], "-g") == 0)
		{
			if(Pidx==0)
			{
				printf("GenerateStitchParams !!\n");
				imgStitch.generateStitchParams();
			}
		}
		else
			imgStitch.run();
	}
	else if(Pcnt==13)
	{
		if(Pidx==0)
		{
			printf("Choose the offline watch !!\n");
		}
		ImageFetch *module; //离线观看
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
	}
	else
 	{
 		printf("Please input the correct process number !!\n");
 	}
	MPI_Finalize();
	printf("END !!\n");
	return(0);
}

//
//int main(int argc, char* argv[]) {
//	MPI_Init(&argc, &argv);
//	int Pcnt, Pidx;
//	MPI_Comm_size(MPI_COMM_WORLD, &Pcnt);
//	MPI_Comm_rank(MPI_COMM_WORLD, &Pidx);
//
//
//	
//	if(Pcnt==13)
//	{
//		ImageFetch *module; //离线观看
//		if(Pidx == CENTER) 
//		{
//			module = new ImageFetchGlobal(Pcnt, Pidx);
//		}		
//		else 
//		{
//			module = new ImageFetchLocal(Pcnt, Pidx, CENTER);
//		}
//		module->Run();
//		delete module;
//	}
//	else if(Pcnt==36)
//	{
//	
//	
//	}
//	else
//	{
//		printf("Please input the correct process number !!\n");
//	}
// 	MPI_Finalize();
//	return(0);
//}
