// ImageStitchOffLine.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ImageStitch.h"


int main(int argc, char* argv[])
{	
	MPI_Init(&argc,&argv);
	ImageStitch imgStitch;
	if (argc == 2 && strcmp(argv[1], "-g") == 0)// ����Ϊ���������ҵڶ���Ϊ'-g'
	{
			printf("generate !!\n");
			imgStitch.generateStitchParams();// ���ɸ��㼶�Ĳ����ļ�
			printf("end !!\n");
	}
	else// �������
	{
		imgStitch.run();// ִ�����к���
	}

	MPI_Finalize();

	return 0;
}

