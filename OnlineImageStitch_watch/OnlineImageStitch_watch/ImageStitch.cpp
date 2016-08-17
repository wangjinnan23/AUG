#include "stdafx.h"
#include "ImageStitch.h"
#include "Message.h"


#define SOCKET_COM 2000
#define SOCKET_BUFFER_LENGTH 256
#define MAX_PATH 255
#define min(x,y) ((x)<(y) ? (x) : (y))

//#ifdef _WINDOWS
//const string ImageStitch::FILENAME_PARAMS = ("D:\\zhou\\ImageStitchOnWindows\\ImageStitchOnlineAndOffline\\x64\\Debug\\ImgStitch.config");
const string ImageStitch::FILENAME_PARAMS = ("E:\\Library\\data\\ImageStitch\\OnlineImageStitch\\config\\ImgStitch.config");
//#else
//const string ImageStitch::FILENAME_PARAMS = "./ImgStitch.config";
//#endif

template<class T>
inline void safeDelete(T* p) {
	if(p != nullptr) {
		delete []p;
		p = nullptr;
	}
}

void pid2gid(IN const int pid, IN const int cameraCnt, IN const int blockCnt, OUT int &gid, OUT int &gType) {
	if (pid < 1) {
		gType = PROCESS_TYPE_COMMUNICATION;
		gid   = pid;
	} else if (pid < 1 + cameraCnt) {
		gType = PROCESS_TYPE_MAP;
		gid   = pid - 1;
	} else if (pid < 1 + cameraCnt + blockCnt) {
		gType = PROCESS_TYPE_REDUCE;
		gid   = pid - 1 - cameraCnt;
	} else {
		gType = PROCESS_TYPE_NULL;
		gid   = pid - 1 - cameraCnt - blockCnt;
	}
	return;
}

int gid2pid(IN const int gid, IN const int gType, IN const int cameraCnt, IN const int blockCnt) {
	int ret = 0;
	switch(gType) {
	case PROCESS_TYPE_COMMUNICATION: ret = gid; break;
	case PROCESS_TYPE_MAP:           ret = gid + 1; break;
	case PROCESS_TYPE_REDUCE:        ret = gid + 1 + cameraCnt; break;
	case PROCESS_TYPE_NULL:          ret = gid + 1 + cameraCnt + blockCnt; break;
	default: break;
	}
	return(ret);
}

BlockParam::BlockParam() {
	m_nMapEntryLocalNum    = 0;	   //It stands for the whole number of the points in the block.
	m_pMapEntryLocalNumVec = nullptr;  //It stands for the 35 cameras points number for the corresponding block.It's stored as the form of (blockParam->m_pMapEntryLocalNumVec)[1-35]
	m_pMapEntryDisp        = nullptr;  //It stands for the offset of the corresponding camera in the block.
	m_nMapEntryGlobalNum   = 0;
	m_pMapTableLocal       = nullptr;
	m_pMapTableGlobal      = nullptr;
}

BlockParam::~BlockParam() {
	safeDelete<MapEntryGlobal>(m_pMapTableGlobal);
	safeDelete<MapEntryLocal>(m_pMapTableLocal);
	safeDelete<int>(m_pMapEntryDisp);
	safeDelete<int>(m_pMapEntryLocalNumVec);
}


ImageStitch::ImageStitch() {
	m_nPcnt                      = 0;  
	m_nPidx                      = 0;
	m_sPname[0]                  = '\0';

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

	m_pBlockParams               = NULL;
	m_pResultLen                 = NULL; //the point number sent to the communication from the cameras  m_pResultLen[camera id]
	m_nBlockRows                 = 0;
	m_nBlockCols                 = 0;
	m_nBlockCnt                  = 0;  //the number of the blocks which consist of the whole picture without overlapping
	m_nGtype                     = 0;
	m_nGidx                      = 0;
	m_nFrameIdx                  = 0;
	m_isOnline                   = false;
	m_nLeft                      = 0;
	m_nRight                     = 0;
	m_nUp                        = 0;
	m_nDown                      = 0;
	m_onlineScale                = 0;

	m_dStartTime  = 0;
	m_dElapseTime = 0;

	
	m_pMapResultVec.clear();
	m_pMapResultGlobal    = NULL;
	m_pReduceResult       = NULL;
	m_pReduceResultGlobal = NULL;

	initMPI();
	printf("Process %d finish mpi init.\n", m_nPidx);
	fflush(stdout);
	loadParams();
	printf("Process %d finish params loading.\n", m_nPidx);
	fflush(stdout);
	MPI_Barrier(MPI_COMM_WORLD);
}

ImageStitch::~ImageStitch() {
	safeDelete<BlockParam>(m_pBlockParams);
	safeDelete<Pixel>(m_pReduceResultGlobal);
	safeDelete<Pixel>(m_pMapResultGlobal);
	safeDelete<int>(m_pResultLen);
	if(!m_pMapResultVec.empty()) {
		for(int i = 0; i < m_pMapResultVec.size(); i++)
			safeDelete<Pixel>(m_pMapResultVec[i]);
		m_pMapResultVec.clear();
	}
	m_imgLocal.release();


}

void ImageStitch::terminate(IN const int errorCode) {
printf("---!!!--Process %d of %d is in a mess .\n", m_nPidx, m_nPcnt);
	fflush(stdout);
	MPI_Abort(MPI_COMM_WORLD, errorCode);
}

void ImageStitch::tic() {
	m_dStartTime = MPI_Wtime();
	return;
}

void ImageStitch::toc() {
	m_dElapseTime = MPI_Wtime() - m_dStartTime;
	return;
}

void ImageStitch::initMPI() 
{

	MPI_Comm_size(MPI_COMM_WORLD, &m_nPcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &m_nPidx);
	int nameLen;
	MPI_Get_processor_name(m_sPname, &nameLen);
	printf("Process %d of %d is on %s.\n", m_nPidx, m_nPcnt, m_sPname);
	fflush(stdout);
	if(sizeof(Pixel) == 1 || sizeof(Pixel) == 3) 
	{
		m_mpiDataTypeForMap    = MPI_UNSIGNED_CHAR;
		m_mpiDataTypeForReduce = MPI_UNSIGNED_CHAR;
	} 
	else if(sizeof(Pixel) == 2 || sizeof(Pixel) == 6) 
	{
		m_mpiDataTypeForMap    = MPI_UNSIGNED_SHORT;
		m_mpiDataTypeForReduce = MPI_UNSIGNED_SHORT;
	} 
	else
	{
		m_mpiDataTypeForMap    = MPI_DATATYPE_NULL;
		m_mpiDataTypeForReduce = MPI_DATATYPE_NULL;
	}
	return;
}

void ImageStitch::loadParams() 
{	
	FileStorage fs(FILENAME_PARAMS, FileStorage::READ);
	if (!fs.isOpened()) 
	{		
		printf("Process %d could not open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	fs["ScaleCnt"]              >> m_nScaleCnt;				//层级数量
	fs["CamaraCnt"]             >> m_nCameraCnt;				//相感器数量
	fs["DirMapTable"]           >> m_sDirMapTable;				//映射表路径
	fs["FileFormatMapTable"]    >> m_sFileFormatMapTable;			//映射表文件名格式
	fs["DirStitchParam"]        >> m_sDirStitchParam;			//层级参数路径
	fs["FileFormatStitchParam"] >> m_sFileFormatStitchParam;		//层级参数文件名格式
	fs["GlobalHeight"]          >> m_nHeightGlobal;				//全图的高
	fs["GlobalWidth"]           >> m_nWidthGlobal;				//全图的宽
	fs["BlockHeight"]           >> m_nHeightBlock;				//分块的高
	fs["BlockWidth"]            >> m_nWidthBlock;				//分块的宽
	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	switch(sizeof(Pixel)) 
	{
	case 1: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_8UC1); break;
	case 3: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_8UC3); break;
	case 2: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_16UC1); break;
	case 6: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_16UC3); break;
	default: break;
	}

//	m_pReduceResult = (Pixel*)m_imgOk.data;


	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	int flag;
	if (m_nGtype == PROCESS_TYPE_MAP) 
	{
		fs["DirLocalImgIn"]        >> m_sDirLocalImgIn;					// <!-- 输入局部图像路径 -->
		fs["FileFormatLocalImgIn"] >> m_sFileFormatLocalImgIn;				// <!-- 局部图像文件名格式 -->
	}
	else if (m_nGtype == PROCESS_TYPE_REDUCE || m_nGtype == PROCESS_TYPE_COMMUNICATION) 	
	{
		fs["DirGlobalImgOut"]        >> m_sDirGlobalImgOut;				//  <!-- 输出全局图像路径 -->
		fs["FileFormatGlobalImgOut"] >> m_sFileFormatGlobalImgOut;			// <!-- 全局图像文件名格式 -->
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
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)	
		m_pResultLen = new int[m_nCameraCnt];
	fs.release();
	return;
}

void ImageStitch::loadStitchParams() 
{
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	char* filename = new char[m_sDirStitchParam.length() + m_sFileFormatStitchParam.length() + MAX_PATH];
	if(filename == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
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
	load(fin);
	fclose(fin);
	safeDelete<char>(filename);
	return;
}

void ImageStitch::load(IN FILE* fin) 
{
	m_pBlockParams = new BlockParam[m_nBlockCnt];
	if(m_pBlockParams == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	if (m_nGtype == PROCESS_TYPE_MAP || m_nGtype == PROCESS_TYPE_REDUCE || m_nGtype == PROCESS_TYPE_COMMUNICATION) 
	{
		for (int i = 0; i < m_nBlockCnt; i++) 
		{
			BlockParam* blockParam = &(m_pBlockParams[i]);
			blockParam->m_pMapEntryLocalNumVec = new int[m_nPcnt];
			if(blockParam->m_pMapEntryLocalNumVec == NULL) 
			{
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			memset(blockParam->m_pMapEntryLocalNumVec, 0, m_nPcnt * sizeof(int));
			for (int j = 0; j < m_nCameraCnt; j++) 
			{
				int temp = gid2pid(j, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
				fread(&((blockParam->m_pMapEntryLocalNumVec)[temp]), sizeof(int),1, fin); 
			}
////更改
///原版			blockParam->m_pMapEntryDisp = new int[m_nPcnt];
blockParam->m_pMapEntryDisp = new int[m_nPcnt+1];

			if(blockParam->m_pMapEntryDisp == NULL) 
			{
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			(blockParam->m_pMapEntryDisp)[0] = 0;
			for (int j = 1; j < m_nPcnt; j++)
				(blockParam->m_pMapEntryDisp)[j] = (blockParam->m_pMapEntryDisp)[j - 1] + (blockParam->m_pMapEntryLocalNumVec)[j - 1];
			blockParam->m_nMapEntryGlobalNum = (blockParam->m_pMapEntryDisp)[m_nPcnt - 1] + (blockParam->m_pMapEntryLocalNumVec)[m_nPcnt - 1]; //blockParam->m_pMapEntryLocalNumVec)[m_nPcnt - 1]=0;
			
/////更改添加一句
(blockParam->m_pMapEntryDisp)[m_nPcnt] = blockParam->m_nMapEntryGlobalNum;
			
			if (m_nGtype == PROCESS_TYPE_MAP) 
			{
				blockParam->m_nMapEntryLocalNum = (blockParam->m_pMapEntryLocalNumVec)[m_nPidx];
				if (blockParam->m_nMapEntryLocalNum > 0) 
				{
					blockParam->m_pMapTableLocal = new MapEntryLocal[blockParam->m_nMapEntryLocalNum];
					blockParam->m_pMapTableGlobal = new MapEntryGlobal[blockParam->m_nMapEntryLocalNum];
					if(blockParam->m_pMapTableLocal == NULL || blockParam->m_pMapTableGlobal == NULL) 
					{
						printf("Process %d could not malloc!\n", m_nPidx);
						fflush(stdout);
						terminate(ERROR_MALLOC);
					}
					for(int k = 0; k < blockParam->m_nMapEntryLocalNum; k++) 
					{
						fread(blockParam->m_pMapTableLocal + k, sizeof(MapEntryLocal), 1, fin);
						fread(blockParam->m_pMapTableGlobal + k, sizeof(MapEntryGlobal), 1, fin);
					}
				}
			} 
			else if ((m_nGtype == PROCESS_TYPE_REDUCE && i == m_nGidx) || m_nGtype == PROCESS_TYPE_COMMUNICATION) 
			{					
				blockParam->m_pMapTableGlobal = new MapEntryGlobal[blockParam->m_nMapEntryGlobalNum];					
				if(blockParam->m_pMapTableGlobal == NULL) 
				{						
					printf("Process %d could not malloc!\n", m_nPidx);
					fflush(stdout);
					terminate(ERROR_MALLOC);
				}
				fread(blockParam->m_pMapTableGlobal, sizeof(MapEntryGlobal), blockParam->m_nMapEntryGlobalNum, fin);
			}
		}
	}
}


void  ImageStitch::initInterVar() 
{
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION) 
	{
#ifdef _WINDOWS
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
#endif
		m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_server == INVALID_SOCKET) 
		{
			printf("Process communication could not set up socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_SETUP);
		}
		printf("Process communication set up socket successfully.\n");
		fflush(stdout);

		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(SOCKET_COM);
		sin.sin_addr.s_addr = INADDR_ANY;
		if(bind(m_server,(LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) 
		{
			printf("Process communication could not bind socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_BIND);
		}
		printf("Process communication bind socket successfully.\n");
		fflush(stdout);

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

Pixelf ImageStitch::biLinInterp(IN const Mat &value, IN const float x, IN const float y) 
{
	//int i = floor(x), j = floor(y);
	//Pixel vA = value.at<uchar>(j, i),
	//      vB = value.at<uchar>(j, i + 1),
	//      vC = value.at<uchar>(j + 1, i),
	//      vD = value.at<uchar>(j + 1, i + 1);
	//Pixelf v;
	//v = (float)vA * (1 + i - x) * (1 + j - y) +
	//    (float)vB * (x - i) * (1 + j - y) +
	//    (float)vC * (1 + i - x) * (y - j) +
	//    (float)vD * (x - i) * (y - j);
	//return(v);

	int i = (int)x;
	int j = (int)y;
	Pixel vA = value.at<uchar>(j, i);
	Pixel vB, vC, vD, vE, vF;
	Pixelf v;

	if (i == x)// x坐标是整数，x方向不需要插值
	{
		vE = vA;
		if (j == y)// y坐标是整数，v直接赋值为vE
		{
			v  = vE;
		}
		else// y坐标不是整数，提取vF并插值
		{
			vF = value.at<float>(j + 1, i);
			v  = (y - j) * (vF - vE) + vE;
		}
	}
	else// x坐标不是整数，x方向先插值
	{
		vB = value.at<float>(j, i + 1);
		vE = (x - i) * (vB - vA) + vA;
		if (j == y)// y坐标是整数，v直接赋值为vE
		{
			v  = vE;
		}
		else// y坐标不是整数，提取vC、vD，插值出vF，再插值出v
		{
			vC = value.at<float>(j + 1, i);
			vD = value.at<float>(j + 1, i + 1);
			vF = (x - i) * (vD - vC) + vC;
			v = (y - j) * (vF - vE) + vE;
		}
	}

	return(v);



}

bool ImageStitch::loadImgModule(IN const int frameIdx) 
{   	//MAP进程

int Block_numbers=0;
	if(m_isOnline) 
	{
		int is_Roc = false;
		int l = m_nLeft / m_nWidthBlock + (m_nUp / m_nHeightBlock) * m_nBlockCols,
		    l_end = (m_nRight - 1) / m_nWidthBlock + ((m_nDown - 1) / m_nHeightBlock) * m_nBlockCols;  //计算起始块和结束块
//printf("Pidx  %d ,块序号　l= %d,l_end= %d .\n",m_nPidx,l,l_end);
//fflush(stdout);
		while(l <= l_end) 
		{
			int i = l;
			for(int j = m_nLeft / m_nWidthBlock; j <= (m_nRight - 1) / m_nWidthBlock; j++, i++)
			{

				if(m_pBlockParams[i].m_nMapEntryLocalNum > 0)     //在该块中的点数大于０
				{
Block_numbers=m_pBlockParams[i].m_nMapEntryLocalNum;
//printf("Pidx  %d ,遍历涉及到的块序号　= %d .\n ",m_nPidx,i);
//fflush(stdout);
					is_Roc = true; break;
				}
			}
			l += m_nBlockCols;			
		}
		if(!is_Roc) 
		{
			int root= gid2pid(0, PROCESS_TYPE_COMMUNICATION, m_nCameraCnt, m_nBlockCnt);
			MPI_Send(&is_Roc, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
			return false;
		}
	}
	char* filename = new char[m_sDirLocalImgIn.length() + m_sFileFormatLocalImgIn.length() + MAX_PATH];
	if(filename == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	sprintf(filename, (m_sDirLocalImgIn + m_sFileFormatLocalImgIn).c_str(), m_nGidx +1, frameIdx);
	m_imgLocal = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_imgLocal.empty()) 
	{
		printf("Process %d could not read image file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	 else 
	{
	//	printf("The image covers %d points !\n",Block_numbers);
		printf("Process %d success read image file: %s!\n", m_nPidx, filename); //读取几号相机图片
		fflush(stdout);
	}
	safeDelete<char>(filename);
	return true;
}

int ImageStitch::mapModule() //MAP模块
{

//	if (m_pMapResultVec.empty())
//	{	
//		m_pMapResultVec.assign(m_nBlockCnt, NULL);
//	}
	for (int i = 0; i < m_pMapResultVec.size(); i++)
		safeDelete<Pixel>(m_pMapResultVec[i]);
	m_pMapResultVec.clear();
	m_pMapResultVec.assign(m_nBlockCnt, NULL);
	
	if(m_isOnline) 
	{
//		safeDelete<Pixel>(m_pMapResultVec[0]);  //?double free
		int Rate = 1 << m_onlineScale;
//////改动
		//m_pMapResultVec[0] = new Pixel[(m_imgLocal.cols / Rate + 1) * (m_imgLocal.rows / Rate + 1)];
m_pMapResultVec[0] = new Pixel[(m_imgLocal.cols ) * (m_imgLocal.rows )];

		Pixel *pResult = m_pMapResultVec[0];
		int l = m_nLeft / m_nWidthBlock + (m_nUp / m_nHeightBlock) * m_nBlockCols,l_end = (m_nRight - 1) / m_nWidthBlock + ((m_nDown - 1) / m_nHeightBlock) * m_nBlockCols;
//tic();
		while(l <= l_end) 
		{
			int i = l;
			for(int j = m_nLeft / m_nWidthBlock; j <= (m_nRight - 1) / m_nWidthBlock; j++, i++) 
			{
				BlockParam* tmp = &(m_pBlockParams[i]);
				for(int idx = 0; idx < tmp->m_nMapEntryLocalNum; idx++) 
				{
					int xGlobal = tmp->m_pMapTableGlobal[idx].xGlobal,
					    yGlobal = tmp->m_pMapTableGlobal[idx].yGlobal,
					    GapX = xGlobal - m_nLeft,
					    GapY = yGlobal - m_nUp;
					if(xGlobal < m_nRight && yGlobal < m_nDown &&  xGlobal >= m_nLeft && yGlobal >= m_nUp )
				    {  //更改  && GapX % Rate == 0 && GapY % Rate == 0
						*(pResult) = biLinInterp(m_imgLocal, tmp->m_pMapTableLocal[idx].xLocal, tmp->m_pMapTableLocal[idx].yLocal);
						pResult++;
					}
				}
			}				
			l += m_nBlockCols;
		}


	//cout<<"pResult="<<long(pResult)<<" , m_pMapResultVec[0] = "<<long(m_pMapResultVec[0])<<" , Pidx = "<<m_nPidx<<endl;
		int root = gid2pid(0, PROCESS_TYPE_COMMUNICATION, m_nCameraCnt, m_nBlockCnt);
		int BufLen = pResult - m_pMapResultVec[0];
//toc();
	fflush(stdout);
//cout<<"before send!"<<"BufLen = "<<BufLen<<",Pidx = "<<m_nPidx<<endl;
		MPI_Send(&BufLen, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
//cout<<"after send!"<<"BufLen = "<<BufLen<<",Pidx = "<<m_nPidx<<endl;
//cout<<"Camera:"<<m_nPidx<<" Elapse Time:"<<m_dElapseTime<<"  .Send "<<setw(10)<<BufLen<<" data to the communication !"<<endl;
		if(BufLen > 0) 
		{
			MPI_Send(m_pMapResultVec[0], BufLen * sizeof(Pixel), m_mpiDataTypeForMap, root, 0, MPI_COMM_WORLD);
cout<<"进程 "<<m_nPidx<<" buflen大于０,给０号进程发送完数据！"<<endl;
		}

safeDelete<Pixel>(m_pMapResultVec[0]);
m_pMapResultVec[0]=NULL;
	}
	return(1);
}

void ImageStitch::gatherModule() 
{

printf("	Pidx is %d . This is the gather Module.\n",m_nPidx);
fflush(stdout);
	if (m_nGtype == PROCESS_TYPE_COMMUNICATION && m_isOnline)
	{
cout<<"０号进程开始进入ｇａｔｈｅｒ模块！"<<endl;
		if (m_pMapResultVec.empty()) m_pMapResultVec.assign(m_nCameraCnt, NULL);
		MPI_Status status;
		for(int i = 0; i < m_nCameraCnt; i++) 
		{
//cout<<"遍历摄像头进程，从中获取数据"<<"摄像头序号　　"<<i<<endl;
			int root = gid2pid(i, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
			MPI_Recv(&(m_pResultLen[i]), 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
//cout<<"Communication: "<<"Receive "<<setw(10)<<m_pResultLen[i]<<" data from the porgress "<<root<<" !"<<endl;
fflush(stdout);
			safeDelete<Pixel>(m_pMapResultVec[i]);
			if(m_pResultLen[i] > 0) 
			{
				m_pMapResultVec[i] = new Pixel[m_pResultLen[i]];
				MPI_Recv(m_pMapResultVec[i], m_pResultLen[i] * sizeof(Pixel), m_mpiDataTypeForMap, root, 0, MPI_COMM_WORLD, &status);
			}
		}
	//	toc(); cout<<"gatherModule- PROCESS_TYPE_COMMUNICATION Elapse　Time:"<<m_dElapseTime<<endl;
	}
}

void ImageStitch::reduceModule() 
{


	int cntPixel = m_imgOk.cols * m_imgOk.rows;

//reduceCnt = NULL;
//resultTemp = NULL;
	 
		

	//ushort * reduceCnt = new ushort[cntPixel];
ushort* reduceCnt = new ushort[cntPixel];

	//Pixelf * resultTemp = new Pixelf[cntPixel];
Pixelf* resultTemp = new Pixelf[cntPixel];



//safeDelete<Pixel>(m_pReduceResult);
m_pReduceResult = new uchar[cntPixel];
memset(m_pReduceResult,0,cntPixel*sizeof(uchar));


//	m_pReduceResult = (Pixel*)m_imgOk.data;

	memset(resultTemp, 0, cntPixel * sizeof(Pixelf));
	memset(reduceCnt, 0, cntPixel * sizeof(ushort));

	if(m_nGtype == PROCESS_TYPE_COMMUNICATION && m_isOnline) 
	{
printf("	Pidx is %d . This is the reduce Module.\n",m_nPidx);

		int Rate = 1 << m_onlineScale;
		int *ResultIdx = new int[m_nCameraCnt];
		memset(ResultIdx, 0, sizeof(int) * m_nCameraCnt);
		int l = m_nLeft / m_nWidthBlock + (m_nUp / m_nHeightBlock) * m_nBlockCols,
		    l_end = (m_nRight - 1) / m_nWidthBlock + ((m_nDown - 1)/ m_nHeightBlock) * m_nBlockCols;
		while(l <= l_end) 
		{
			int i = l;


			for(int j = m_nLeft / m_nWidthBlock; j <= (m_nRight - 1) / m_nWidthBlock; j++, i++) 
			{
				BlockParam* tmp = &(m_pBlockParams[i]);
//tic();
				for(int pid = 0; pid <= m_nPcnt; pid++)  //这是计算的
				{
					int gType, gid;
					pid2gid(pid, m_nCameraCnt, m_nBlockCnt, gid, gType);
					if(gType == PROCESS_TYPE_MAP && ResultIdx[gid] < m_pResultLen[gid]) 
					{
						int idx = tmp->m_pMapEntryDisp[pid];
						while(idx < tmp->m_pMapEntryDisp[pid + 1] && ResultIdx[gid] < m_pResultLen[gid]) 
						{
							int xGlobal = tmp->m_pMapTableGlobal[idx].xGlobal,
							    yGlobal = tmp->m_pMapTableGlobal[idx].yGlobal,
							    GapX = xGlobal - m_nLeft,
							    GapY = yGlobal - m_nUp;
							if(xGlobal < m_nRight && yGlobal < m_nDown &&
							   xGlobal >= m_nLeft && yGlobal >= m_nUp )
							{  //&& GapX % Rate == 0 && GapY % Rate == 0
							//	int mapIdx = GapY / Rate * m_imgOk.cols + GapX / Rate;
								////更改
								int mapIdx = GapY  * m_imgOk.cols + GapX;

								resultTemp[mapIdx] += (m_pMapResultVec[gid])[ResultIdx[gid]];
								reduceCnt[mapIdx]++;
					    
								ResultIdx[gid]++;

							}
							idx++;
						}
					} 
					else if(gType == PROCESS_TYPE_REDUCE) break;
				}
//toc(); cout<<"Block:"<<i<<" Elapse Time:"<<m_dElapseTime<<endl;
			}	
			l += m_nBlockCols;
		}

//for (int i = 1; i < m_pMapResultVec.size(); i++)
//    safeDelete<Pixel>(m_pMapResultVec[i]);
m_pMapResultVec.clear();
m_pMapResultVec.assign(m_nBlockCnt, NULL);

		safeDelete<int>(ResultIdx);
	}


	for(int idx = 0; idx < cntPixel; idx++)
{
	if(reduceCnt[idx] != 0) 
		{	
			m_pReduceResult[idx] = (resultTemp[idx] / reduceCnt[idx]);
		}
	else 
		{
			m_pReduceResult[idx] = 0;
    	}
//cout<<" idx = "<<idx<<",m_pReduceResult[idx] = "<<int(m_pReduceResult[idx])<<"  ";
}


safeDelete<ushort>(reduceCnt);
safeDelete<Pixelf>(resultTemp);

return;
}

void ImageStitch::ImageEachScale(IN const int frameIdx) 
{
if(m_nPidx==0)
{
printf("	Pidx is %d . This is the Scale Module.\n",m_nPidx);
}
if(m_nPidx==1)
{
printf("	Pidx is %d . This is the Scale Module. ",m_nPidx);
} 
if(m_nPidx==36)
{
printf("	Pidx is %d . This is the Scale Module.\n",m_nPidx);
}
	char* filename = new char[m_sDirGlobalImgOut.length() + m_sFileFormatGlobalImgOut.length() + MAX_PATH];
	if(filename == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}

	Mat tmpImg[2];
	tmpImg[0] = m_imgOk.clone();
	int i = 0;
	for(int ScaleIdx = 0; ScaleIdx < m_nScaleCnt; ScaleIdx++) 
	{
		sprintf(filename, (m_sDirGlobalImgOut + m_sFileFormatGlobalImgOut).c_str(), ScaleIdx, m_nGidx, frameIdx);
		imwrite(filename, tmpImg[i]);
		resize(tmpImg[i], tmpImg[1 - i], Size(tmpImg[i].cols / 2, tmpImg[i].rows / 2),(0,0),(0,0),CV_INTER_AREA);
		i = 1 - i;
	}
	safeDelete<char>(filename);
	return;
}

void ImageStitch::run() 
{
	loadStitchParams();
	printf("Process %d finish image stitch params loading.\n", m_nPidx);
	fflush(stdout);
	initInterVar();
	printf("Process %d finish intermediate variable init.\n", m_nPidx);
	fflush(stdout);
	char recvBuf[MAX_PATH];
	int isRun = 0, cnt = 0, *temp = (int*)recvBuf;
	memset(recvBuf, 0, sizeof(recvBuf));
	int root = gid2pid(0,PROCESS_TYPE_COMMUNICATION, m_nCameraCnt,m_nBlockCnt), onlineRoot;
	MPI_Comm comm_online;
	MPI_Comm_split(MPI_COMM_WORLD, (m_nGtype == PROCESS_TYPE_COMMUNICATION) || (m_nGtype == PROCESS_TYPE_MAP), 0, &comm_online);//MPI_Comm_split [0 1 2...35][36...134]  brooadcast the roc in the communicate and map modules.
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
		MPI_Comm_rank(comm_online, &onlineRoot);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&onlineRoot, 1, MPI_INT, root, MPI_COMM_WORLD);
	if(m_nGtype == PROCESS_TYPE_REDUCE) 
	{
		while(true) 
		{
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(!isRun) continue;
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(cnt < sizeof(int)) continue;
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(temp[0] == STITCH_ONLINE) 
			{
				m_isOnline = true;
				MPI_Barrier(MPI_COMM_WORLD);
			} 
		}
	} 
	else if(m_nGtype == PROCESS_TYPE_MAP) 
	{
		while(true) 
		{
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(!isRun) continue;
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(cnt < sizeof(int)) continue;
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(temp[0] == STITCH_ONLINE) 
			{
				m_isOnline = true;
				while(true) 
				{

					MPI_Barrier(comm_online);
					MPI_Bcast(&cnt, 1, MPI_INT, onlineRoot, comm_online);
					if(cnt < int(8 * sizeof(int)) || temp[0] != STITCH_ONLINE) break;
					MPI_Bcast(&m_nFrameIdx, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nLeft, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nRight, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nUp, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nDown, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_onlineScale, 1, MPI_INT, onlineRoot, comm_online);
//printf("  Pidx is %d . This is the map Module in the loop .\n",m_nPidx);

//cout<<"最后处理时的显示区域： "<<" 左 "<<m_nLeft<<", 右 "<<m_nRight<<", 上 "<<m_nUp<<", 下 "<<m_nDown<<"层级："<<m_onlineScale<<endl;

					if(loadImgModule(m_nFrameIdx))
						mapModule();
				}
		//		cout<<"同步之前 ,进程 "<<m_nPidx<<endl;

				MPI_Barrier(MPI_COMM_WORLD);
			} 
		}
	} 
	else if(m_nGtype == PROCESS_TYPE_COMMUNICATION) 
	{
		struct sockaddr_in remoteAddr;
		int addrLen = sizeof(remoteAddr);
		while(true) 
		{///
			printf("Waiting for client of ExcProcess to connect.\n");
			fflush(stdout);
			m_client_exc = accept(m_server, (LPSOCKADDR)&remoteAddr, (socklen_t*)&addrLen);
			if(m_client_exc == INVALID_SOCKET) 
			{
				printf("Process communication fails to accept a connect!\n");
				fflush(stdout);
				isRun = 0;
			} 
			else 
			{
				printf("Process communication accept a connect: %s.\n", inet_ntoa(remoteAddr.sin_addr));
				fflush(stdout);
				isRun = 1;
			}
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast((void*)&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(!isRun) continue;
			cnt = recv(m_client_exc, recvBuf, SOCKET_BUFFER_LENGTH, 0);
/////
cout<<"\n\n0The number from the client sending : "<<cnt<<"\n recvBuf "<<" temp[0] "<<temp[0]<<", temp[1] "<<temp[1]<<" 左 "<<temp[4]<<", 右 "<<temp[5]<<", 上 "<<temp[6]<<", 下 "<<temp[7]<<endl;
int SendTimes= 0;
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(cnt < sizeof(int)) 
			{
				
				closesocket(m_client_exc); 
				continue;
			}
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(temp[0] == STITCH_ONLINE) 
			{
				m_isOnline = true;
				int ScreenWidth, ScreenHeight;
cout<<"在循环外，第一次！\n";
				while(true) 
				{
cout<<"\n在循环内，循环中！\n";	
                   tic();  //开始计时
					cnt = recv(m_client_exc, recvBuf, SOCKET_BUFFER_LENGTH, 0);
///
					toc();
					cout<<"接收完客户端参数时耗时: "<<m_dElapseTime<<endl;


cout<<"The number from the client sending : "<<cnt<<"\n显示区域： "<<" 左 "<<temp[4]<<", 右 "<<temp[5]<<", 上 "<<temp[6]<<", 下 "<<temp[7]<<endl;



					MPI_Barrier(comm_online);
					MPI_Bcast(&cnt, 1, MPI_INT, onlineRoot, comm_online);

//					if( (cnt < int(8 * sizeof(int))) ||(temp[0] != STITCH_ONLINE))
					if( (cnt < int(8 * sizeof(int))))
					{  
						break;
					}
				
		
					m_nFrameIdx = temp[1];
					ScreenWidth = temp[2];
					ScreenHeight = temp[3];
					m_nLeft = temp[4];
					m_nRight = temp[5];
					m_nUp = temp[6];
					m_nDown = temp[7];
					m_onlineScale = 0;

					if((m_nLeft<=0)||(m_nRight>=12720)||(m_nUp<=0)||(m_nDown>=15000))
					{
					cout<<"边界处理："<<endl;
					//防止越界
					if(m_nLeft<=0)
					m_nLeft=0;
					if(m_nLeft>= 12720)
					m_nLeft=12720 - 1000;
					if(m_nRight>=12720)
					m_nRight=12720;
					if(m_nUp<=0)
					m_nUp=0;
					if(m_nUp>=15000)
					m_nUp = 15000-1000;
					if(m_nDown>=15000)
					m_nDown = 15000;
					}

					int Rate = min((m_nRight - m_nLeft) / ScreenWidth, (m_nDown - m_nUp) / ScreenHeight);
					while(Rate > 1) {Rate = Rate >> 1; m_onlineScale++;}
					if(m_onlineScale > m_nScaleCnt) m_onlineScale = m_nScaleCnt;
					MPI_Bcast(&m_nFrameIdx, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nLeft, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nRight, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nUp, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_nDown, 1, MPI_INT, onlineRoot, comm_online);
					MPI_Bcast(&m_onlineScale, 1, MPI_INT, onlineRoot, comm_online);

					//int width = (m_nRight - m_nLeft) / (1 << m_onlineScale),
					//    height = (m_nDown - m_nUp) / (1 << m_onlineScale);

					////更改 
					int width = (m_nRight - m_nLeft),
					   height = (m_nDown - m_nUp);

					m_imgOk = Mat(height, width, CV_8UC1);
					gatherModule();

					printf("  Pidx is %d . This is the gather Module in the loop .\n",m_nPidx);

					reduceModule();
	
	//Mat dst(m_imgOk.rows , m_imgOk.cols, CV_8UC1);cout<<"即将保存"<<endl;
	//dst.setTo(0);
	//


	//	cout<<"m_pReduceResult[0]"<<(int)m_pReduceResult[0]<<"m_pReduceResult[223963]"<<(int)m_pReduceResult[223963]<<",(int)m_pReduceResult[223964]="<<(int)m_pReduceResult[223964]<<endl;
	//	cout<<"m_pReduceResult[223964]地址："<<(int)&(m_pReduceResult[223964])<<endl;


					Mat dst = Mat(m_imgOk.rows, m_imgOk.cols, CV_8UC1,(void*)m_pReduceResult);
imwrite("E:\\Workspace\\VS2010\\Result_data\\online_store_image\\BeforeResingSending.jpg",dst);
			//		imwrite("D:\\dst.png",dst);
			//		Mat dst = Mat( m_imgOk.rows, m_imgOk.cols, CV_8UC1,imagetmp);
			//		Mat dst = Mat(ScreenHeight, ScreenWidth, CV_8UC1);
				//	if(m_imgOk.cols != ScreenWidth || m_imgOk.rows != ScreenHeight)
				//	{
					resize(dst, dst, Size(ScreenWidth, ScreenHeight),(0,0),(0,0),CV_INTER_AREA);
						//resize(m_imgOk, dst, Size(ScreenWidth, ScreenHeight),0,0,CV_INTER_LINEAR);
				//	}
	
				//	else
				//{
				//	dst = m_imgOk;
				//}


					char* sendBufdata = (char*)(dst.data);
					send(m_client_exc, sendBufdata, sizeof(Pixel) * ScreenWidth * ScreenHeight, 0);

					toc(); //结束计时
					printf("Total time to complete the image: %f s\n", m_dElapseTime);

					//imwrite("D:\\m_imgSend.png",dst);
					SendTimes++;
					cout<<"The times of your sending pic  :    "<<SendTimes<<endl;

				}
				MPI_Barrier(MPI_COMM_WORLD);
				
			} 
			closesocket(m_client_exc);
		}
		closesocket(m_server);
//#ifdef _WINDOWS
		WSACleanup();
//#endif
	}
	printf("Process %d finish running.\n", m_nPidx);
	fflush(stdout);
	MPI_Comm_free(&comm_online);
	return;
}

void ImageStitch::generateStitchParams() 
{	
	printf("Process %d starts generating scale parameters.\n", m_nPidx);
	fflush(stdout);
	tic();
	char* filename = new char[m_sDirMapTable.length() + MAX_PATH];
	if(filename == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	sprintf(filename, "%s", (m_sDirMapTable + m_sFileFormatMapTable).c_str());
	FILE* fin = fopen(filename, "rb");
	if (fin == nullptr) 
	{
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	int fileCnt = m_nCameraCnt + m_nBlockCnt + 1; //36+98+1;
	FILE** fout = new FILE*[fileCnt];
	if(fout == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++) //给135个文件取名
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

	int cnt;
	fread(&cnt, sizeof(int), 1, fin);//映射表的点数
	MapTableEntry* mapTable = new MapTableEntry[cnt];
	if(mapTable == NULL) 
	{
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	fread(mapTable, sizeof(MapTableEntry), cnt, fin);
	fclose(fin);
	for (int i = 0; i < m_nBlockRows; i++) 
	{
		for (int j = 0; j < m_nBlockCols; j++) 
		{
			int blockIdx = i * m_nBlockCols + j;
			int xLeftTop = m_nWidthBlock * j, yLeftTop = m_nHeightBlock * i;
			int xRightBottom = min(xLeftTop + m_nWidthBlock - 1, m_nWidthGlobal), yRightBottom = min(yLeftTop + m_nHeightBlock - 1, m_nHeightGlobal);
			int* mapEntryLocalNumVec = new int[m_nCameraCnt];
			if(mapEntryLocalNumVec == NULL) 
			{
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			memset(mapEntryLocalNumVec, 0, m_nCameraCnt * sizeof(int));
			for (int k = 0; k < cnt; k++)
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
				    (mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))
					mapEntryLocalNumVec[mapTable[k].cameraIdx]++;
			for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
				fwrite(mapEntryLocalNumVec, sizeof(int), m_nCameraCnt, fout[fileIdx]);
			safeDelete<int>(mapEntryLocalNumVec);

			for (int k = 0; k < cnt; k++) 
			{
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
				    (mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom)) 
				{
					int fileIdx = gid2pid(mapTable[k].cameraIdx, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
					MapEntryLocal tmpLocal(mapTable[k].xLocal, mapTable[k].yLocal);
					MapEntryGlobal tmpGlobal(mapTable[k].xGlobal, mapTable[k].yGlobal);
					fwrite(&tmpLocal, sizeof(MapEntryLocal), 1, fout[fileIdx]);
					fwrite(&tmpGlobal, sizeof(MapEntryGlobal), 1, fout[fileIdx]);

					fileIdx = gid2pid(0, PROCESS_TYPE_COMMUNICATION, m_nCameraCnt, m_nBlockCnt);
					fwrite(&tmpGlobal, sizeof(MapEntryGlobal), 1, fout[fileIdx]);

					fileIdx = gid2pid(blockIdx, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);
					tmpGlobal.xGlobal -= xLeftTop;
					tmpGlobal.yGlobal -= yLeftTop;
					fwrite(&tmpGlobal, sizeof(MapEntryGlobal), 1, fout[fileIdx]);
				}
			}
		}
	}

	safeDelete<MapTableEntry>(mapTable);
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
		fclose(fout[fileIdx]);
	safeDelete<FILE*>(fout);
	safeDelete<char>(filename);
	toc();
	printf("Process %d finishes generating scale parameters.\n", m_nPidx);
	printf("Time: %f\n", m_dElapseTime);
	fflush(stdout);
	return;
}