// 读取，创建文本文件.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;
#define readbuline
int _tmain(int argc, _TCHAR* argv[])
{
	ofstream File_write;
	string path="./file.bin";
	File_write.open(path,ios::out|ios::binary);
	if(File_write.is_open())
	{
		cout<<"This file is opened successfully for writing !\n";
		for(int i=0;i<5;i++)
		File_write<<i<<"write this to file \r\n";
		File_write<<"    end writing this to file \r\n";
		File_write.close();
	}
	else
	{
		cout<<"Unable to open the file for writing !";
	}
	 
	ifstream File_read;
	string line;
	path="E:/Workspace/VS2010/Aug/OfflineImageStitch/Block_Cam_points.txt";
	File_read.open(path,ios::in|ios::binary);
	if(File_read.is_open())
	{

		cout<<"This file is opened successfully for reading !\n";
#ifdef readbuline //逐行读入，显示 
		while( getline (File_read,line))
		{
			cout<<"\r\n\n该行的长度是 "<<(line.length())<<"\r\n";
			cout<<line<<endl;
			int i=0;
			while(i!=line.length())
			{
				cout<<line[i++]<< " ";
			}
		
		}
		File_read.close();
#else		//逐字读入，并显示



#endif
	}
	else
	{
		cout<<"Unable to open the file for reading !";
	}
	system("pause");
	return 0;
}

