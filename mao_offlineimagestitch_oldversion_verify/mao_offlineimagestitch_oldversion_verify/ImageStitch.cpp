/*********************************************************************//**
* \file      ImageStitch.cpp
* \brief     图像拼接类的实现
* \author    陈宏钊、穆楠、周茂民
* \version   1.0b
* \date      2016年4月
* \copyright 该文件版权归清华大学所有
**************************************************************************/

#include "stdafx.h"
#include "ImageStitch.h"


const string ImageStitch::FILENAME_PARAMS =("E:\\Library\\data\\Verification\\Mao_offlineimageStitch\\ImgStitch.config");

/*********************************************************************//**
* \brief   进程编号到组编号的转换
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
*
* \param   pid       进程编号
* \param   cameraCnt 像感器数量
* \param   blockCnt  块数量
* \param   gid       组编号
* \param   gType     组类型
*
* \sa      gid2pid(), GroupType
**************************************************************************/
void pid2gid(IN const int pid, IN const int cameraCnt, IN const int blockCnt, OUT int &gid, OUT int &gType)
{
	if (pid < 1)
	{
		gType = PROCESS_TYPE_COMMUNICATION;
		gid   = pid;
	}
	else if (pid < 1 + cameraCnt)
	{
		gType = PROCESS_TYPE_MAP;
		gid   = pid - 1;
	}
	else if (pid < 1 + cameraCnt + blockCnt)
	{
		gType = PROCESS_TYPE_REDUCE;
		gid   = pid - 1 - cameraCnt;
	}
	else
	{
		gType = PROCESS_TYPE_NULL;
		gid   = pid - 1 - cameraCnt - blockCnt;
	}

	return;
}

/*********************************************************************//**
* \brief   组编号到进程编号的转换
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
*
* \param   gid       组编号
* \param   gType     组类型
* \param   cameraCnt 像感器数量
* \param   blockCnt  块数量
* \return  进程编号
*
* \sa      pid2gid(), GroupType
**************************************************************************/
int gid2pid(IN const int gid, IN const int gType, IN const int cameraCnt, IN const int blockCnt)
{
	int ret = 0;
	switch(gType)
	{
	case PROCESS_TYPE_COMMUNICATION:
		ret = gid;
		break;
	case PROCESS_TYPE_MAP:
		ret = gid + 1;
		break;
	case PROCESS_TYPE_REDUCE:
		ret = gid + 1 + cameraCnt;
		break;
	case PROCESS_TYPE_NULL:
		ret = gid + 1 + cameraCnt + blockCnt;
		break;
	default:
		break;
	}
	return(ret);
}


/*********************************************************************//**
* \brief   BlockParam类的构造函数
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
*
* \sa      ~BlockParam()
**************************************************************************/
BlockParam::BlockParam()
{
	m_nMapEntryLocalNum    = 0;
	m_pMapEntryLocalNumVec = nullptr;
	m_pMapEntryDisp        = nullptr;
	m_nMapEntryGlobalNum   = 0;
	m_pMapTableLocal       = nullptr;
	m_pMapTableGlobal      = nullptr;
}

/*********************************************************************//**
* \brief   BlockParam类的析构函数
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
*
* \sa      BlockParam()
**************************************************************************/
BlockParam::~BlockParam()
{
	if (m_pMapTableGlobal != nullptr)
	{
		delete []m_pMapTableGlobal;
		m_pMapTableGlobal = nullptr;
	}
	
	if (m_pMapTableLocal != nullptr)
	{
		delete []m_pMapTableLocal;
		m_pMapTableLocal = nullptr;
	}
	if (m_pMapEntryDisp != nullptr)
	{
		delete []m_pMapEntryDisp;
		m_pMapEntryDisp = nullptr;
	}
	if (m_pMapEntryLocalNumVec != nullptr)
	{
		delete []m_pMapEntryLocalNumVec;
		m_pMapEntryLocalNumVec = nullptr;
	}
}


/*********************************************************************//**
* \brief   ImageStitch类的构造函数
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      ~ImageStitch(), initMPI(), loadParams()
**************************************************************************/
ImageStitch::ImageStitch()
{
	m_nPcnt						 = 0;
	m_nPidx						 = 0;
	m_sPname[0]					 = '\0';

	m_nScaleCnt                  = 0;
	m_nCameraCnt                 = 0;
	m_sDirLocalImgIn             = "";
	m_sFileFormatLocalImgIn      = "";
	m_sDirGlobalImgOut           = "";
	m_sFileFormatGlobalImgOut    = "";
	m_sDirMapTable               = "";
	m_sFileFormatMapTable        = "";
	m_sDirStitchParam            = "";
	m_sFileFormatStitchParam     = "";

	m_pBlockParams				 = nullptr;				
	m_nBlockRows			     = 0;
	m_nBlockCols                 = 0;
	m_nBlockCnt                  = 0;
	m_nGtype                     = 0;
	m_nGidx                      = 0;
	m_nFrameCnt                  = 0;
	m_nFrameBegin				 = 0;
	m_nFrameEnd					 = 0;
	m_nDataIdx					 = 0;

	m_dStartTime  = 0;
	m_dElapseTime = 0;

	m_pMapResultVec.clear();
	m_pMapResultGlobal    = nullptr;
	m_pReduceResult       = nullptr;
	m_pReduceResultGlobal = nullptr;

	initMPI();
	printf("Process %d finish mpi init.\n", m_nPidx);
	fflush(stdout);

	loadParams();
	printf("Process %d finish params loading.\n", m_nPidx);
	fflush(stdout);

	MPI_Barrier(MPI_COMM_WORLD);
}

/*********************************************************************//**
* \brief   ImageStitch类的构造函数
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      ImageStitch()
**************************************************************************/
ImageStitch::~ImageStitch()
{
	//m_imgGlobal.release();
	if(m_pBlockParams != nullptr)
	{
		delete []m_pBlockParams;
		m_pBlockParams = nullptr;
	}
	if (m_pReduceResultGlobal != nullptr)
	{
		delete []m_pReduceResultGlobal;
		m_pReduceResultGlobal = nullptr;
	}
	if (m_pReduceResult != nullptr)
	{
		delete []m_pReduceResult;
		m_pReduceResult = nullptr;
	}
	if (m_pMapResultGlobal != nullptr)
	{
		delete []m_pMapResultGlobal;
		m_pMapResultGlobal = nullptr;
	}
	if (!m_pMapResultVec.empty())
	{
		for (int i = 0; i < m_pMapResultVec.size(); i++)
		{
			if (m_pMapResultVec[i] != nullptr)
			{
				delete [](m_pMapResultVec[i]);
				m_pMapResultVec[i] = nullptr;
			}
		}
		m_pMapResultVec.clear();
	}
	m_imgLocal.release();
}

/*********************************************************************//**
* \brief   异常退出
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
*
* \param   errorCode 错误代码
*
* \sa      ErrorType
**************************************************************************/
void ImageStitch::terminate(IN const int errorCode)
{
	fflush(stdout);
	MPI_Abort(MPI_COMM_WORLD, errorCode);
}


/*********************************************************************//**
* \brief   开始计时
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::tic()
{
	m_dStartTime = MPI_Wtime();
	return;
}

/*********************************************************************//**
* \brief   停止计时并记录流逝时间
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::toc()
{
	m_dElapseTime = MPI_Wtime() - m_dStartTime;
	return;
}


/*********************************************************************//**
* \brief   初始化mpi相关内容
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::initMPI()
{
	// 得到进程数量、进程编号和处理器名称
	MPI_Comm_size(MPI_COMM_WORLD, &m_nPcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &m_nPidx);
	
//	m_nPidx = 60;
	
	int nameLen;
	MPI_Get_processor_name(m_sPname, &nameLen);
	printf("Process %d of %d is on %s.\n", m_nPidx, m_nPcnt, m_sPname);
	fflush(stdout);

	// 设置mpi数据类型用于mpi通信
	m_mpiDataTypeForMap    = MPI_UNSIGNED_SHORT;
	m_mpiDataTypeForReduce = MPI_UNSIGNED_SHORT;

	return;
}

/*********************************************************************//**
* \brief   读取给定参数
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      pid2gid()
**************************************************************************/
void ImageStitch::loadParams()
{
	// 打开参数文件
	FileStorage fs(FILENAME_PARAMS, FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("Process %d could not open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
		printf("Process %d open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
	// 读入层级数量、像感器数量
	fs["ScaleCnt"]  >> m_nScaleCnt;
	fs["CamaraCnt"] >> m_nCameraCnt;

	// 读入映射表所在目录、映射表文件名格式、层级参数所在目录、层级参数文件名格式
	fs["DirMapTable"]				 >> m_sDirMapTable;
	fs["FileFormatMapTable"]		 >> m_sFileFormatMapTable;
	fs["DirStitchParam"]			 >> m_sDirStitchParam;
	fs["FileFormatStitchParam"]   >> m_sFileFormatStitchParam;

			
	cout<<m_sDirMapTable<<endl;
	cout<<m_sDirStitchParam<<endl;

	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	// 更新当前进程所属组的类型及编号
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);

	int flag;
	if (m_nGtype == PROCESS_TYPE_MAP)// map进程
	{
		// 读入输入的局部图像所在目录、输入的局部图像文件名格式
		fs["DirLocalImgIn"]        >> m_sDirLocalImgIn;
		fs["FileFormatLocalImgIn"] >> m_sFileFormatLocalImgIn;
	}

	if (m_nGtype == PROCESS_TYPE_REDUCE)// communication进程
	{
		// 读入输出的全局图像所在目录、输出的全局图像文件名格式
		fs["DirGlobalImgOut"]        >> m_sDirGlobalImgOut;
		fs["FileFormatGlobalImgOut"] >> m_sFileFormatGlobalImgOut;

				// 创建该输出目录并判断是否成功，如果失败则退出
//#ifdef _WINDOWS
		_mkdir(m_sDirGlobalImgOut.c_str());
		flag = _access(m_sDirGlobalImgOut.c_str(), 0);
//#else
//		mkdir(m_sDirGlobalImgOut.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//		flag = access(m_sDirGlobalImgOut.c_str(), F_OK);
//#endif
		if (flag != 0)
		{
			printf("Process %d could not find directory for output global images!\n", m_nPidx);
			fflush(stdout);
			terminate(ERROR_FIND_DIR);
		}
	}


	fs.release();
	return;

}

/*********************************************************************//**
* \brief   载入拼接参数
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      pid2gid(), ImageStitch::set(), ImageStitch::load()
**************************************************************************/
void ImageStitch::loadStitchParams()
{

	// 更新当前进程的组类型及编号
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);

	// 打开本进程对应的参数文件，如失败则退出
	char* filename = new char[m_sDirStitchParam.length() + 260];
	if(filename == NULL)
	{
		return;
	}
	FILE*  fin;
	sprintf(filename, (m_sDirStitchParam + m_sFileFormatStitchParam).c_str(), m_nGtype, m_nGidx);
	fin = fopen(filename, "rb");
	if (fin == nullptr)
	{
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	printf("Process %d open file: %s!\n", m_nPidx, filename);

	// 对该层级的参数进行设置和载入，然后关闭文件
	set(m_nPcnt, m_nPidx, m_nCameraCnt, m_nBlockRows, m_nBlockCols);
	load(fin);
	fclose(fin);
	safeDelete<char>(filename);
	return;
}

/*********************************************************************//**
* \brief   ImageStitch类设置参数，
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   processCnt 进程数量
* \param   processIdx 进程编号
* \param   camaraCnt  像感器数量
* \param   blockRows  行方向的块数量
* \param   blockCols  列方向的块数量
*
* \sa      pid2gid()
**************************************************************************/
void ImageStitch::set(IN const int processCnt, IN const int processIdx, IN const int cameraCnt, IN const int blockRows, IN const int blockCols)
{	
	m_nPcnt = processCnt;
	m_nPidx = processIdx;
	m_nCameraCnt = cameraCnt;
	m_nBlockRows = blockRows;
	m_nBlockCols = blockCols;
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	return;
}

/*********************************************************************//**
* \brief   ImageStitch类的载入函数
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   fin 载入文件指针
**************************************************************************/
void ImageStitch::load(IN FILE* fin)
{
	// 给块参数数组分配空间
	m_pBlockParams = new BlockParam[m_nBlockCnt];
	if(m_pBlockParams == NULL)
	{
		return;
	}
	for (int i = 0; i < m_nBlockCnt; i++)// 逐个块进行处理
	{
		BlockParam* blockParam = &(m_pBlockParams[i]);
		if (m_nGtype == PROCESS_TYPE_MAP || m_nGtype == PROCESS_TYPE_REDUCE)// map或reduce进程
		{
			// 读入“局部映射表条目数量数组”
			blockParam->m_pMapEntryLocalNumVec = new int[m_nPcnt];
			if(blockParam->m_pMapEntryLocalNumVec == NULL)
			{
				return;
			}
			memset(blockParam->m_pMapEntryLocalNumVec, 0, m_nPcnt * sizeof(int));
			for (int j = 0; j < m_nCameraCnt; j++)// 文件中只存储对应每个像感器的条目数量，因为其他的并没有
			{    //每个块都读入对应每个像感器的条目数量
				int temp = gid2pid(j, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);// 此处必须做从组编号到进程编号的映射
				fread(&((blockParam->m_pMapEntryLocalNumVec)[temp]), sizeof(int),1, fin);
			}

			// 根据“局部映射表条目数量数组”计算“局部映射表对应全局映射表中的位置数组”
			blockParam->m_pMapEntryDisp = new int[m_nPcnt];
			if(blockParam->m_pMapEntryDisp == NULL)
			{
				return;
			}
			(blockParam->m_pMapEntryDisp)[0] = 0;
			for (int j = 1; j < m_nPcnt; j++)
			{
				(blockParam->m_pMapEntryDisp)[j] = (blockParam->m_pMapEntryDisp)[j-  1] + (blockParam->m_pMapEntryLocalNumVec)[j - 1];
			}

			// 总数目即为“全局映射表条目数量”
			blockParam->m_nMapEntryGlobalNum = (blockParam->m_pMapEntryDisp)[m_nPcnt - 1] + (blockParam->m_pMapEntryLocalNumVec)[m_nPcnt - 1];

		}

		if (m_nGtype == PROCESS_TYPE_MAP)// map进程
		{
			blockParam->m_nMapEntryLocalNum = (blockParam->m_pMapEntryLocalNumVec)[m_nPidx];
			if (blockParam->m_nMapEntryLocalNum > 0)// 映射到当前块的数目不为0时才读取局部映射表
			{
				blockParam->m_pMapTableLocal = new MapEntryLocal[blockParam->m_nMapEntryLocalNum];
				if(blockParam->m_pMapTableLocal == NULL)
				{
					return;
				}
				fread(blockParam->m_pMapTableLocal, sizeof(MapEntryLocal), blockParam->m_nMapEntryLocalNum, fin);
			}
		}

		if (m_nGtype == PROCESS_TYPE_REDUCE)// reduce进程
		{
			if (i == m_nGidx)// 只有块序号与当前reduce进程的块序号一致时才读取全局映射表
			{
				blockParam->m_pMapTableGlobal = new MapEntryGlobal[blockParam->m_nMapEntryGlobalNum];
				if(blockParam->m_pMapTableGlobal == NULL)
				{
					return;
				}
				fread(blockParam->m_pMapTableGlobal, sizeof(MapEntryGlobal), blockParam->m_nMapEntryGlobalNum, fin);
			}
		}
	}
}


/*********************************************************************//**
* \brief   初始化中间变量
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
* \sa      pid2gid(), GroupType
**************************************************************************/
void  ImageStitch::initInterVar()
{
	// 进行相关参数的更新
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
	{
//ifdef _WINDOWS
		// 开启套接字服务
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if(WSAStartup(sockVersion, &wsaData) != 0)
		{
			printf("Process communication could not start up WSA.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_WSA_STARTUP);
		}
		printf("Process communication starts up WSA successfully.\n");
		fflush(stdout);
//#endif

		// 创建server socket
		m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_server == INVALID_SOCKET)
		{
			printf("Process communication could not set up socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_SETUP);
		}
		printf("Process communication set up socket successfully.\n");
		fflush(stdout);
		
		//绑定server
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(2000);
		sin.sin_addr.s_addr = INADDR_ANY;
		if(bind(m_server,(LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			printf("Process communication could not bind socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_BIND);
		}
		printf("Process communication bind socket successfully.\n");
		fflush(stdout);

		//开始监听
		if(listen(m_server, 5) == SOCKET_ERROR)
		{
			printf("Process communication could not listen.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_LISTEN);
		}
		printf("Process communication starts listening successfully.\n");
		fflush(stdout);
	}
	return;
}

/*********************************************************************//**
* \brief   双线性插值
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   value 值矩阵
* \param   x     X坐标
* \param   y     Y坐标
*
* \return  插值结果
**************************************************************************/
Pixel32 ImageStitch::biLinInterp(IN const Mat &value, IN const float x, IN const float y)
{	
//	printf("Process %d is in biLinInterp.\n",m_nPidx);
	//value.convertTo(value,CV_32FC1);
	
	int i = (int)x;
	int j = (int)y;
	float vA = (float)value.at<uchar>(j, i);
//	float vA = value.at<float>(j, i);
	float vB, vC, vD, vE, vF;
	Pixel32 v;

	if (i == x)// x坐标是整数，x方向不需要插值
	{
		vE = vA;
		if (j == y)// y坐标是整数，v直接赋值为vE
		{
			v.datatmp  = vE;
		}
		else// y坐标不是整数，提取vF并插值
		{
			vF = (float)value.at<uchar>(j + 1, i);
//			vF = value.at<float>(j + 1, i);
			v.datatmp  = (y - j) * (vF - vE) + vE;
		}
	}
	else// x坐标不是整数，x方向先插值
	{
//		vB = value.at<float>(j, i + 1);
		vB = (float)value.at<uchar>(j, i + 1);
		vE = (x - i) * (vB - vA) + vA;
		if (j == y)// y坐标是整数，v直接赋值为vE
		{
			v.datatmp  = vE;
		}
		else// y坐标不是整数，提取vC、vD，插值出vF，再插值出v
		{
			vC = (float)value.at<uchar>(j + 1, i);
			vD = (float)value.at<uchar>(j + 1, i + 1);
//						vC = value.at<float>(j + 1, i);
//						vD = value.at<float>(j + 1, i + 1);
			vF = (x - i) * (vD - vC) + vC;
			v.datatmp  = (y - j) * (vF - vE) + vE;
		}
	}
	//printf("Process %d is in bilinInterp.\n",m_nPidx);
	return(v);
}



/*********************************************************************//**
* \brief   map函数
* \author  陈宏钊、穆楠、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   imgLocal  局部图像
* \param   mapTable  局部映射表
* \param   cnt       局部映射表长度
* \param   mapResult map结果
*
* \return  map结果长度
*
* \sa      biLinInterp()
**************************************************************************/
int ImageStitch::map(IN const Mat &imgLocal, IN const MapEntryLocal* mapTable, IN const int cnt, OUT Pixel* &mapResult)
{
	// 检查mapResult是否为空指针，不是则释放内存并赋值为空指针
	if (mapResult != nullptr)
	{
		delete []mapResult;
		mapResult = nullptr;
	}

	// 映射表条目数为0，直接返回
	if (cnt == 0)
	{
		return(0);
	}
	//printf("Process %d is in map.\n",m_nPidx);

	// 逐项进行双线性插值映射
	mapResult = new Pixel[cnt];
	for (int idx = 0; idx < cnt; idx++)
	{
		mapResult[idx].Pixel32to16( biLinInterp(imgLocal, mapTable[idx].xLocal, mapTable[idx].yLocal));

	}
	///

	// 返回mapResult的数组长度，等于映射表的长度
	return(cnt);
}




/*********************************************************************//**
* \brief   reduce函数
* \author  陈宏钊、穆楠、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   mapResult     map结果
* \param   mapTable      全局映射表
* \param   cntTableEntry 全局映射表长度
* \param   row           分块图像行数
* \param   col           分块图像列数
* \param   reduceResult  分块reduce结果
**************************************************************************/
void ImageStitch::reduce(IN const Pixel* mapResult, IN const MapEntryGlobal* mapTable, IN const int cntTableEntry, IN const int row, IN const int col, OUT Pixel* &reduceResult)
{
	// 检查reduceResult是否为空指针，不是则释放内存并赋值为空指针
	if (reduceResult != nullptr)
	{
		delete []reduceResult;
		reduceResult = nullptr;
	}

	// 为结果及中间结果分配空间
	int cntPixel = row * col;//某一块所在大图总点数
	reduceResult  = new Pixel[cntPixel];
	Pixel32* resultTemp = new Pixel32[cntPixel];
	ushort* reduceCnt = new ushort[cntPixel];
	if(reduceResult == 0 || resultTemp == 0 || reduceCnt == 0)
	{
		return;
	}
	memset(reduceResult, 0, cntPixel * sizeof(Pixel));
	memset(resultTemp, 0, cntPixel * sizeof(Pixel32));
	memset(reduceCnt, 0, cntPixel * sizeof(ushort));

	// 逐项进行加法，注意循环次数为cntTableEntry而不是cntPixel  ，cntTableEntry是全局映射表的长度
	for (int idx = 0; idx < cntTableEntry; idx++)
	{
		resultTemp[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal]= resultTemp[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal] + mapResult[idx];
		//mapTable[idx].yGlobal * col + mapTable[idx].xGlobal是把二维坐标点转化到一维数组中对应位置
		reduceCnt[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal]++;//相应点处的像素数目加1
	}


	// 逐项进行除法，注意循环次数为cntPixel而不是cntTableEntry
	for (int idx = 0; idx < cntPixel; idx++)
	{
		if (reduceCnt[idx] != 0)// 注意这里必须判0，否则会出现除0的错误
		{
			reduceResult[idx].Pixel32to16((resultTemp[idx] / reduceCnt[idx]));//求平均值，做归约
		}
	}

	safeDelete<ushort>(reduceCnt);
	safeDelete<Pixel32>(resultTemp);
	return;
}

/*********************************************************************//**
* \brief   载入图像模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   frameIdx 帧序号
**************************************************************************/
void ImageStitch::loadImgModule(IN const int frameIdx)
{
	if (frameIdx < m_nFrameBegin || frameIdx > m_nFrameEnd)// 帧序号无效，异常退出
	{
		printf("Process %d tempts to read image file of invalid frame %d!\n", m_nPidx, frameIdx);
		fflush(stdout);
		terminate(ERROR_INVALID_FRAME);
	}

	// 生成文件名并读入图像，判断是否为空，如果为空则异常退出
	char* filename = new char[m_sDirLocalImgIn.length() + 260];
	if(filename == NULL)
	{
		return;
	}
	int CIdx = m_nGidx +1;  //组编号为0—（cameraCnt-1），而读入图像片时，按照1-cameraCnt

	/*sprintf(filename, (m_sDirLocalImgIn + m_sFileFormatLocalImgIn).c_str(),CIdx, m_nDataIdx,frameIdx);*/

	sprintf(filename, (m_sDirLocalImgIn + m_sFileFormatLocalImgIn).c_str(), CIdx, frameIdx);

	m_imgLocal = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);


    if (m_imgLocal.empty())
	{
		printf("Process %d could not read image file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	else
	{	
		printf("Process %d success read image file: %s!\n", m_nPidx, filename);	
		fflush(stdout);
	}

	// 转换为浮点数类型
	//m_imgLocal.convertTo(m_imgLocal, CV_32FC1);
	return;
}

/*********************************************************************//**
* \brief   map模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      map()
**************************************************************************/
int ImageStitch::mapModule()
{
	if (!m_pMapResultVec.empty())// 单像感器map结果数组非空，逐项释放内存，然后清空
	{
		for (int i = 0; i < m_pMapResultVec.size(); i++)
		{
			if (m_pMapResultVec[i] != nullptr)
			{
				delete [](m_pMapResultVec[i]);
				m_pMapResultVec[i] = nullptr;
			}
		}
		m_pMapResultVec.clear();
	}
	m_pMapResultVec.assign(m_nBlockCnt, (Pixel*)nullptr);// 分配长度并全部赋值为空指针

	for (int i = 0; i < m_nBlockCnt; i++)// 根据需要的块逐块进行map操作
	{
	
		BlockParam* tmp = &(m_pBlockParams[i]);// 获取相应的块参数
		map(m_imgLocal, tmp->m_pMapTableLocal, tmp->m_nMapEntryLocalNum, m_pMapResultVec[i]);// 进行map操作
//		printf("Process %d  m_nMapEntryGlobalNum is :%d",m_nPidx,tmp->m_pMapTableLocal);
	}

	return(1);
}

/*********************************************************************//**
* \brief   gather模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::gatherModule()
{
	if (m_nGtype == PROCESS_TYPE_MAP)// map进程
	{

		for (int i = 0; i < m_nBlockCnt; i++)// 逐块进行gather
		{
			BlockParam* tmp = &(m_pBlockParams[i]);
			int root = gid2pid(i, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);//该root为当前块号对应的进程号，即reduce模块，tmp->m_nMapEntryLocalNum有可能为0，为0表示该map进程不发送数据
			MPI_Gatherv((void*)m_pMapResultVec[i], tmp->m_nMapEntryLocalNum, m_mpiDataTypeForMap, (void*)nullptr,
				tmp->m_pMapEntryLocalNumVec, tmp->m_pMapEntryDisp, m_mpiDataTypeForMap, root, MPI_COMM_WORLD);
		}
	}
	else if (m_nGtype == PROCESS_TYPE_REDUCE)// reduce进程
	{
			BlockParam* tmp = &(m_pBlockParams[m_nGidx]);
			int root = m_nPidx;//当前的进程号，即当前reduce进程进行收集
			if (m_pMapResultGlobal != nullptr)
			{
				delete []m_pMapResultGlobal;
				m_pMapResultGlobal = nullptr;
			}
			m_pMapResultGlobal = new Pixel[tmp->m_nMapEntryGlobalNum];  
			if(m_pMapResultGlobal == NULL)
			{
				return;
			}
			MPI_Gatherv((void*)nullptr, 0, m_mpiDataTypeForMap, (void*)m_pMapResultGlobal,
				tmp->m_pMapEntryLocalNumVec, tmp->m_pMapEntryDisp, m_mpiDataTypeForMap, root, MPI_COMM_WORLD);
		
	}
	return;
}


/*********************************************************************//**
* \brief   reduce模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      reduce()
**************************************************************************/
void ImageStitch::reduceModule()
{
		BlockParam* tmp = &(m_pBlockParams[m_nGidx]);
		int row = m_nHeightBlock;// 获取该分块的行数
		int col = m_nWidthBlock;// 获取该分块的列数
		reduce(m_pMapResultGlobal, tmp->m_pMapTableGlobal, tmp->m_nMapEntryGlobalNum, row, col, m_pReduceResult);// 进行reduce操作

	return;
}

/*********************************************************************//**
* \brief   存储图像模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
*
* \param   ScaleIdx 层级序号 frameIdx 帧序号
**************************************************************************/
void ImageStitch::saveImgModule(IN const int ScaleIdx,IN const int frameIdx , IN const Mat &Img )
{
	if (frameIdx <= 0 || frameIdx > m_nFrameCnt)// 帧序号无效，异常退出
	{
		printf("Process %d tempts to save image file of invalid frame %d!\n", m_nPidx, frameIdx);
		fflush(stdout);
		terminate(ERROR_INVALID_FRAME);
	}
	//生成图像存储路径
	char* filename = new char[m_sDirGlobalImgOut.length() + 260];
	if(filename == NULL)
	{
		return;
	}
	sprintf(filename, (m_sDirGlobalImgOut + m_sFileFormatGlobalImgOut).c_str(), ScaleIdx, m_nGidx, frameIdx);

//	printf("out file name:%s\n", filename);
//	fflush(stdout);
	
	imwrite(filename, Img);// 存储图像
	return;
}


/*********************************************************************//**
* \brief   生成每一层级的图像
* \author  周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::ImageEachScale(IN const int frameIdx)
{	
	int BlockPixelCnt = m_nHeightBlock * m_nWidthBlock;
	ushort* tmp = new ushort[BlockPixelCnt];
	if(tmp == NULL)
	{
		return;
	}
	memset(tmp, 0, BlockPixelCnt * sizeof(ushort));
	for(int i = 0; i<BlockPixelCnt; i++)
	{
		tmp[i] = m_pReduceResult[i].data;
	}
    m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock,CV_16UC1,tmp);//生成最高分辨率的块
	m_imgOk.convertTo(m_imgOk, CV_8UC1);


	Mat src = m_imgOk.clone();
	Mat dst;
	for(int idx = 0; idx < m_nScaleCnt; idx++)  //生成每一层级的分块图像并保存
	{
		saveImgModule(idx, frameIdx, src);
		resize(src, dst, Size(src.cols / 2, src.rows / 2),(0,0),(0,0),CV_INTER_AREA);
		src = dst;
	}

	safeDelete<ushort>(tmp);
	return;
}



/*********************************************************************//**
* \brief   接收拼接参数模块
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
int ImageStitch::recvStitchParams()
{
	int cnt;
	char recvBuf[MAX_PATH];
	memset(recvBuf, 0, sizeof(recvBuf));
	
	int root = gid2pid(0,PROCESS_TYPE_COMMUNICATION, m_nCameraCnt,m_nBlockCnt);

	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
	{
		cnt = recv(m_client_exc, recvBuf, 255, 0);
	}


	MPI_Bcast((void*)&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);
	if(cnt <= 0)
	{
		return(-1);
	}


	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
	{
		cout<<"receive information from client is :"<<recvBuf<<endl;
		m_nFrameBegin  = atoi(recvBuf);   //起始帧
		m_nFrameCnt	   = atoi(&recvBuf[2]);   //需要拼接的帧数量
		m_nDataIdx	   = atoi(&recvBuf[4]);   //通知载入图像模块取编号为m_nDataIdx文件里面的图像数据
		m_nFrameEnd    = m_nFrameBegin + m_nFrameCnt - 1;
		printf("FrameBegin: %d, FrameEnd: %d , FrameCnt: %d\n", m_nFrameBegin, m_nFrameEnd,m_nFrameCnt);
		fflush(stdout);
	}



	MPI_Bcast((void*)&m_nFrameBegin, 1,MPI_INT, root, MPI_COMM_WORLD);
	if(m_nFrameBegin <= 0)
	{
		return(1);
	}

	MPI_Bcast((void*)&m_nFrameCnt, 1, MPI_INT, root, MPI_COMM_WORLD);
	if(m_nFrameCnt <= 0)
	{
		return(2);
	}

	MPI_Bcast((void*)&m_nDataIdx, 1, MPI_INT, root, MPI_COMM_WORLD);
	if(m_nDataIdx < 0)
	{
		return(3);
	}

	//MPI_Bcast((void*)&m_nFrameEnd,1,MPI_INT, root, MPI_COMM_WORLD);
	//if(m_nFrameEnd < m_nFrameBegin)
	//{
	//	return(4);
	//}

	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	return(0);
}


/*********************************************************************//**
* \brief   发送拼接完成信息
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::sendStitchFinish()
{
	char sendBuf[256] = "ImageStitch offline is finished!\n ";
	cout<<sendBuf<<endl;
	send(m_client_exc, sendBuf, 256, 0);
}



/*********************************************************************//**
* \brief   运行函数
* \author  陈宏钊 、周茂民
* \version 1.0b
* \date    2016年4月
*
* \sa      loadScaleParams(), initInterVar()
**************************************************************************/
void ImageStitch::run()
{
	
	// 读入各层级参数
	loadStitchParams();
	printf("Process %d finish image stitch params loading.\n", m_nPidx);
	fflush(stdout);

	// 初始化中间变量
	initInterVar();
	printf("Process %d finish intermediate variable init.\n", m_nPidx);
	fflush(stdout);

	struct sockaddr_in remoteAddr;
	int addrLen = sizeof(remoteAddr);
	int isRun = 0;
	int root = gid2pid(0,PROCESS_TYPE_COMMUNICATION, m_nCameraCnt,m_nBlockCnt);

	while(true)
	{
		MPI_Barrier(MPI_COMM_WORLD);

		if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
		{
			printf("Waiting for client of ExcProcess to connect.\n");
			fflush(stdout);
			m_client_exc = accept(m_server, (LPSOCKADDR)&remoteAddr, (socklen_t*)&addrLen);
			if(m_client_exc == INVALID_SOCKET)
			{
				printf("Process communication fails to accept a connect!\n");
				fflush(stdout);
				isRun = 0;//isRun标记为0，循环继续，继续等待请求连接
			}
			else//连接请求接受成功，输出ip地址，isRun标记为1
			{
				printf("Process communication accept a connect: %s.\n", inet_ntoa(remoteAddr.sin_addr));
				fflush(stdout);
				isRun = 1;
			}
		}

		// 广播isRun，根据它的值决定是否进行下一步
		MPI_Bcast((void*)&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);
		if(isRun == 0)
		{
			continue;
		}
	

		MPI_Barrier(MPI_COMM_WORLD);
	
		int flag = recvStitchParams();// 从显示客户端接收参数（起始和结束帧，是否强制停止标志）
		if(flag == -1)// 接收不到数据，说明client已断开连接,重新等待连接
		{
			continue;
		}
		else if(flag != 0)
		{
			if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
			{
				printf("Process communication receive wrong parameters, wrong positon is %d!\n", flag);
				fflush(stdout);
			}
		}
		else // 接收参数成功
		{	m_nFrameEnd = m_nFrameBegin + m_nFrameCnt - 1;
			for(int frameIdx = m_nFrameBegin; frameIdx <= m_nFrameEnd; frameIdx++)
			{	
				MPI_Barrier(MPI_COMM_WORLD);
				if (m_nGtype == PROCESS_TYPE_MAP)
					{	
						loadImgModule(frameIdx);

						tic();
						mapModule();
						toc();
						printf("Process %d finish mapping, elapse time is %lf\n", m_nPidx, m_dElapseTime);
						fflush(stdout);

						gatherModule();

					}
				else if (m_nGtype == PROCESS_TYPE_REDUCE) //处理生成图像，存储在本地
					{
						tic();
						gatherModule();
						toc();
						printf("Process %d finish gathering, elapse time is %lf\n", m_nPidx, m_dElapseTime);
						fflush(stdout);
						
						tic();
						reduceModule();
						toc();
						printf("Process %d finish reducing, elapse time is %lf\n", m_nPidx, m_dElapseTime);
						fflush(stdout);

						tic();
						ImageEachScale(frameIdx);
						toc();
						printf("Process %d finish ImageEachScale,elapse time is %lf.\n", m_nPidx, m_dElapseTime);
					}
			}
		}

	
		MPI_Barrier(MPI_COMM_WORLD);
		// 离线拼接完成，communication进程将该客户端对应的socket关闭，再次循环，等待下一次拼接请求
		if (m_nGtype == PROCESS_TYPE_COMMUNICATION)
		{
				//回馈客户端拼接完成信息
		      sendStitchFinish();
		      closesocket(m_client_exc);
		}
	}
	// 从循环中退出，则关闭服务端对应的socket，并清空套接字服务
	if (m_nGtype == PROCESS_TYPE_COMMUNICATION)
	{
		closesocket(m_server);
		WSACleanup();
	}
		printf("Process %d finish running off-line.\n", m_nPidx);
		fflush(stdout);
    return;
}



/*********************************************************************//**
* \brief   生成各层级的参数文件
* \author  陈宏钊
* \version 1.0b
* \date    2016年4月
**************************************************************************/
void ImageStitch::generateStitchParams()
{	
	printf("Process %d starts generating scale parameters.\n", m_nPidx);
	fflush(stdout);

	char* filename = new char[m_sDirMapTable.length() + 260];//定义文件名指针
	if(filename == NULL)
	{
		return;
	}
//#ifdef _WINDOWS
	_mkdir(m_sDirStitchParam.c_str());
//#else
//	mkdir(m_sDirScaleParam.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//#endif
	sprintf(filename, (m_sDirMapTable + m_sFileFormatMapTable).c_str());//把文件名目录写到filename中，用于在下一步打开文件
	FILE* fin = fopen(filename, "rb");//打开文件filename，以二进制形式读取
	if (fin == nullptr)//判断是否为空指针
	{
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}

	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;//分块的总数目
	int fileCnt = m_nCameraCnt + m_nBlockCnt + 1;//文件数目
	FILE** fout = new FILE*[fileCnt];
	if(fout == NULL)
	{
		return;
	}
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)//fileIdx表示第idx个文件
	{
		int groupType, groupIdx;
		pid2gid(fileIdx, m_nCameraCnt, m_nBlockCnt, groupIdx, groupType);
		sprintf(filename, (m_sDirStitchParam + m_sFileFormatStitchParam).c_str(), groupType, groupIdx);
		fout[fileIdx] = fopen(filename, "wb");
		if (fout[fileIdx] == nullptr)
		{
			printf("Process %d could not open file: %s!\n", m_nPidx, filename);
			fflush(stdout);
			terminate(ERROR_OPEN_FILE);
		}
	}

	int cnt;//表示原始映射表条目MapTableEntry总数目
	fread(&cnt, sizeof(int), 1, fin);
	MapTableEntry* mapTable = new MapTableEntry[cnt];  //maptable表示全局映射表
	if(mapTable == NULL)
	{
		return;
	}
	fread(mapTable, sizeof(MapTableEntry), cnt, fin);//从文件中读入原始映射表到MapTable
	fclose(fin);

	for (int i = 0; i < m_nBlockRows; i++)  //行方向的分块数m_nBlockRows
	{
		for (int j = 0; j < m_nBlockCols; j++)
		{
			int blockIdx = i * m_nBlockCols + j;//
			int xLeftTop = m_nWidthBlock * j, yLeftTop = m_nHeightBlock * i;//第j块左上角坐标
			int xRightBottom = min(xLeftTop + m_nWidthBlock - 1, m_nWidthGlobal), yRightBottom = min(yLeftTop + m_nHeightBlock - 1, m_nHeightGlobal);
			//右下角坐标
			// 记录当前块从每个像感器收集的像素数量
			int* mapEntryLocalNumVec = new int[m_nCameraCnt];
			if(mapEntryLocalNumVec == NULL)
			{
				return;
			}
			memset(mapEntryLocalNumVec, 0, m_nCameraCnt * sizeof(int));
			for (int k = 0; k < cnt; k++)
			{
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
					(mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))//判断坐标是否落入相应的块内
				{
					mapEntryLocalNumVec[mapTable[k].cameraIdx]++;//在该块内则该像感器收集的像素数量+1
				}
			}
			for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
			 {
				int groupType, groupIdx;
				pid2gid(fileIdx, m_nCameraCnt, m_nBlockCnt, groupIdx, groupType);
				if (groupType == PROCESS_TYPE_MAP || groupType == PROCESS_TYPE_REDUCE)
				{
					fwrite(mapEntryLocalNumVec, sizeof(int), m_nCameraCnt, fout[fileIdx]);//将收集到的每个像感器的像素数目写入对应的文件
				}
			 }

			safeDelete<int>(mapEntryLocalNumVec);

			for (int k = 0; k < cnt; k++)//按顺序遍历所有的点
			{
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
					(mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))//判断坐标是否落入相应的块内
				{
					int fileIdx = gid2pid(mapTable[k].cameraIdx, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
					MapEntryLocal tmpLocal(mapTable[k].xLocal, mapTable[k].yLocal);
					fwrite(&tmpLocal, sizeof(MapEntryLocal), 1, fout[fileIdx]);//Map只需要写入局部坐标，因为只用到的局部坐标
					fileIdx = gid2pid(blockIdx, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);
					MapEntryGlobal tmpGlobal(mapTable[k].xGlobal - xLeftTop, mapTable[k].yGlobal - yLeftTop);
					fwrite(&tmpGlobal, sizeof(MapEntryGlobal), 1, fout[fileIdx]);//reduce只用到了全局坐标，只写入全局坐标
				}
			}
		}
	}

	safeDelete<MapTableEntry>(mapTable);
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
	{
		fclose(fout[fileIdx]);
	}
	delete []fout;
//	safeDelete<FILE*>(fout);
	
	printf("Process %d finishes generating scale parameters.\n", m_nPidx);
	fflush(stdout);

	safeDelete<char>(filename);
	return;
}