// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define _WINDOWS


#ifdef _WINDOWS
#include <iomanip>
#endif
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>
#include<io.h>
#include<direct.h>
#include<WinSock2.h>
#include<omp.h>
#include<mpi.h>
#include<opencv\cxcore.h>
#include<opencv\highgui.h>
#include<opencv2\opencv.hpp>

typedef uchar Pixel;
typedef int socklen_t;

#define min(x,y)	(x<y?x:y)
#define max(x,y)	(x>y?x:y)

#define OFFLINE_WATCH 0x00
#define ONLINE_WATCH 0x01
#define OFFLINE_STITCH 0x02

using namespace cv;
using namespace std;