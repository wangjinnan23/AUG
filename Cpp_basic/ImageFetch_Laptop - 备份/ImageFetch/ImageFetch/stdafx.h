// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define _WINDOWS


#ifdef _WINDOWS
#include <iomanip>
#endif
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 在此处引用程序需要的其他头文件
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