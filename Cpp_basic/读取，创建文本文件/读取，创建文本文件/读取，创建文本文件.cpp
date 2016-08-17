// 读取，创建文本文件.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;

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
	File_read.open(path,ios::in|ios::binary);
	if(File_read.is_open())
	{

		cout<<"This file is opened successfully for reading !\n";
		while( getline (File_read,line))
		{
			cout<<line<<endl;
		}
		File_read.close();
	}
	else
	{
		cout<<"Unable to open the file for reading !";
	}
	return 0;
}

