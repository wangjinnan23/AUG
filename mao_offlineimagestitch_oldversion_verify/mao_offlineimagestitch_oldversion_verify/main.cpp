// ImageStitchOffLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ImageStitch.h"


int main(int argc, char* argv[])
{	
	MPI_Init(&argc,&argv);
	ImageStitch imgStitch;
	if (argc == 2 && strcmp(argv[1], "-g") == 0)// 参数为两个，并且第二个为'-g'
	{
			printf("generate !!\n");
			imgStitch.generateStitchParams();// 生成各层级的参数文件
			printf("end !!\n");
	}
	else// 其他情况
	{
		imgStitch.run();// 执行运行函数
	}

	MPI_Finalize();

	return 0;
}

