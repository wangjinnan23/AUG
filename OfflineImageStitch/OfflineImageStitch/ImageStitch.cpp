#include "ImageStitch.h"
#include "Message.h"
#include "stdafx.h"
#define SOCKET_COM 2000
#define SOCKET_BUFFER_LENGTH 256
#define MAX_PATH 255
#define min(x,y) ((x)<(y) ? (x) : (y))

#ifdef _WINDOWS
const string ImageStitch::FILENAME_PARAMS = "E:\\Library\\data\\ImageStitch\\OfflineImageStitch\\config\\ImgStitch.config";
#else
const string ImageStitch::FILENAME_PARAMS = "./ImgStitch.config";
#endif

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
	m_nMapEntryLocalNum    = 0;
	m_pMapEntryLocalNumVec = nullptr;
	m_pMapEntryDisp        = nullptr;
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
	m_pResultLen                 = NULL;
	m_nBlockRows                 = 0;
	m_nBlockCols                 = 0;
	m_nBlockCnt                  = 0;
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

void ImageStitch::initMPI() {
	MPI_Comm_size(MPI_COMM_WORLD, &m_nPcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &m_nPidx);
	int nameLen;
	MPI_Get_processor_name(m_sPname, &nameLen);
	printf("Process %d of %d is on %s.\n", m_nPidx, m_nPcnt, m_sPname);
	fflush(stdout);
	if(sizeof(Pixel) == 1 || sizeof(Pixel) == 3) {
		m_mpiDataTypeForMap    = MPI_UNSIGNED_CHAR;
		m_mpiDataTypeForReduce = MPI_UNSIGNED_CHAR;
	} else if(sizeof(Pixel) == 2 || sizeof(Pixel) == 6) {
		m_mpiDataTypeForMap    = MPI_UNSIGNED_SHORT;
		m_mpiDataTypeForReduce = MPI_UNSIGNED_SHORT;
	} else {
		m_mpiDataTypeForMap    = MPI_DATATYPE_NULL;
		m_mpiDataTypeForReduce = MPI_DATATYPE_NULL;
	}
	return;
}

void ImageStitch::loadParams() {
	FileStorage fs(FILENAME_PARAMS, FileStorage::READ);
	if (!fs.isOpened()) {
		printf("Process %d could not open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	fs["ScaleCnt"]              >> m_nScaleCnt;
	fs["CamaraCnt"]             >> m_nCameraCnt;
	fs["DirMapTable"]           >> m_sDirMapTable;
	fs["FileFormatMapTable"]    >> m_sFileFormatMapTable;
	fs["DirStitchParam"]        >> m_sDirStitchParam;
	fs["FileFormatStitchParam"] >> m_sFileFormatStitchParam;
	fs["GlobalHeight"]          >> m_nHeightGlobal;
	fs["GlobalWidth"]           >> m_nWidthGlobal;
	fs["BlockHeight"]           >> m_nHeightBlock;
	fs["BlockWidth"]            >> m_nWidthBlock;
	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	switch(sizeof(Pixel)) {
	case 1: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_8UC1); break;
	case 3: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_8UC3); break;
	case 2: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_16UC1); break;
	case 6: m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock, CV_16UC3); break;
	default: break;
	}
	m_pReduceResult = (Pixel*)m_imgOk.data;
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	int flag;
	if (m_nGtype == PROCESS_TYPE_MAP) {
		fs["DirLocalImgIn"]        >> m_sDirLocalImgIn;
		fs["FileFormatLocalImgIn"] >> m_sFileFormatLocalImgIn;
	}else if (m_nGtype == PROCESS_TYPE_REDUCE || m_nGtype == PROCESS_TYPE_COMMUNICATION) {
		fs["DirGlobalImgOut"]        >> m_sDirGlobalImgOut;
		fs["FileFormatGlobalImgOut"] >> m_sFileFormatGlobalImgOut;
#ifdef _WINDOWS
		_mkdir(m_sDirGlobalImgOut.c_str());
		flag = _access(m_sDirGlobalImgOut.c_str(), 0);
#else
		mkdir(m_sDirGlobalImgOut.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		flag = access(m_sDirGlobalImgOut.c_str(), F_OK);
#endif
		if (flag != 0) {
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

void ImageStitch::loadStitchParams() {
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	char* filename = new char[m_sDirStitchParam.length() + m_sFileFormatStitchParam.length() + MAX_PATH];
	if(filename == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	FILE*  fin;
	sprintf(filename, (m_sDirStitchParam + m_sFileFormatStitchParam).c_str(), m_nGtype, m_nGidx);
	fin = fopen(filename, "rb");
	if (fin == nullptr) {
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	load(fin);
	fclose(fin);
	safeDelete<char>(filename);
	return;
}

void ImageStitch::load(IN FILE* fin) {
	m_pBlockParams = new BlockParam[m_nBlockCnt];
	if(m_pBlockParams == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	if (m_nGtype == PROCESS_TYPE_MAP || m_nGtype == PROCESS_TYPE_REDUCE || m_nGtype == PROCESS_TYPE_COMMUNICATION) {
		for (int i = 0; i < m_nBlockCnt; i++) {
			BlockParam* blockParam = &(m_pBlockParams[i]); //对前者的赋值，目的在于通过指针对后者赋值
			blockParam->m_pMapEntryLocalNumVec = new int[m_nPcnt];
			if(blockParam->m_pMapEntryLocalNumVec == NULL) {
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			memset(blockParam->m_pMapEntryLocalNumVec, 0, m_nPcnt * sizeof(int));
			for (int j = 0; j < m_nCameraCnt; j++) {
				int temp = gid2pid(j, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
				fread(&((blockParam->m_pMapEntryLocalNumVec)[temp]), sizeof(int),1, fin);
			}
			blockParam->m_pMapEntryDisp = new int[m_nPcnt];
			if(blockParam->m_pMapEntryDisp == NULL) {
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			(blockParam->m_pMapEntryDisp)[0] = 0;
			for (int j = 1; j < m_nPcnt; j++)
				(blockParam->m_pMapEntryDisp)[j] = (blockParam->m_pMapEntryDisp)[j - 1] + (blockParam->m_pMapEntryLocalNumVec)[j - 1];
			blockParam->m_nMapEntryGlobalNum = (blockParam->m_pMapEntryDisp)[m_nPcnt - 1] + (blockParam->m_pMapEntryLocalNumVec)[m_nPcnt - 1];
			if (m_nGtype == PROCESS_TYPE_MAP) {
				blockParam->m_nMapEntryLocalNum = (blockParam->m_pMapEntryLocalNumVec)[m_nPidx];
				if (blockParam->m_nMapEntryLocalNum > 0) {
					blockParam->m_pMapTableLocal = new MapEntryLocal[blockParam->m_nMapEntryLocalNum];
					blockParam->m_pMapTableGlobal = new MapEntryGlobal[blockParam->m_nMapEntryLocalNum];
					if(blockParam->m_pMapTableLocal == NULL || blockParam->m_pMapTableGlobal == NULL) {
						printf("Process %d could not malloc!\n", m_nPidx);
						fflush(stdout);
						terminate(ERROR_MALLOC);
					}
					for(int k = 0; k < blockParam->m_nMapEntryLocalNum; k++) {
						fread(blockParam->m_pMapTableLocal + k, sizeof(MapEntryLocal), 1, fin);
						fread(blockParam->m_pMapTableGlobal + k, sizeof(MapEntryGlobal), 1, fin);
					}
				}
			} else if ((m_nGtype == PROCESS_TYPE_REDUCE && i == m_nGidx) || m_nGtype == PROCESS_TYPE_COMMUNICATION) {
					blockParam->m_pMapTableGlobal = new MapEntryGlobal[blockParam->m_nMapEntryGlobalNum];
					if(blockParam->m_pMapTableGlobal == NULL) {
						printf("Process %d could not malloc!\n", m_nPidx);
						fflush(stdout);
						terminate(ERROR_MALLOC);
					}
					fread(blockParam->m_pMapTableGlobal, sizeof(MapEntryGlobal), blockParam->m_nMapEntryGlobalNum, fin);
			}
		}
	}
}


void  ImageStitch::initInterVar() {
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION) {
#ifdef _WINDOWS
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if(WSAStartup(sockVersion, &wsaData) != 0) {
			printf("Process communication could not start up WSA.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_WSA_STARTUP);
		}
		printf("Process communication starts up WSA successfully.\n");
		fflush(stdout);
#endif
		m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_server == INVALID_SOCKET) {
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
		if(bind(m_server,(LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
			printf("Process communication could not bind socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_BIND);
		}
		printf("Process communication bind socket successfully.\n");
		fflush(stdout);

		if(listen(m_server, 5) == SOCKET_ERROR) {
			printf("Process communication could not listen.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_LISTEN);
		}
		printf("Process communication starts listening successfully.\n");
		fflush(stdout);
	}
	return;
}

Pixelf ImageStitch::biLinInterp(IN const Mat &value, IN const float x, IN const float y) {
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

bool ImageStitch::loadImgModule(IN const int frameIdx) {
	char* filename = new char[m_sDirLocalImgIn.length() + m_sFileFormatLocalImgIn.length() + MAX_PATH];
	if(filename == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	sprintf(filename, (m_sDirLocalImgIn + m_sFileFormatLocalImgIn).c_str(), m_nGidx +1, frameIdx);
	m_imgLocal = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	if (m_imgLocal.empty()) {
		printf("Process %d could not read image file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	} else {
		printf("Process %d success read image file: %s!\n", m_nPidx, filename);
		fflush(stdout);
	}
	safeDelete<char>(filename);
	return true;
}

int ImageStitch::mapModule() {
	if (m_pMapResultVec.empty())
		m_pMapResultVec.assign(m_nBlockCnt, NULL);
	else if (m_pMapResultVec.size() < m_nBlockCnt) {
		for (int i = 0; i < m_pMapResultVec.size(); i++)
			safeDelete<Pixel>(m_pMapResultVec[i]);
		m_pMapResultVec.clear();
		m_pMapResultVec.assign(m_nBlockCnt, NULL);
	}
	if(!m_isOnline)
		for (int i = 0; i < m_nBlockCnt; i++) {
			BlockParam* tmp = &(m_pBlockParams[i]);
			int cnt = tmp->m_nMapEntryLocalNum;
			safeDelete<Pixel>(m_pMapResultVec[i]);
			if (cnt == 0)  continue;
			m_pMapResultVec[i] = new Pixel[cnt];
			for (int idx = 0; idx < cnt; idx++)
				(m_pMapResultVec[i])[idx] = biLinInterp(m_imgLocal, tmp->m_pMapTableLocal[idx].xLocal, tmp->m_pMapTableLocal[idx].yLocal);
		}
	return(1);
}

void ImageStitch::gatherModule() {
	if (m_nGtype == PROCESS_TYPE_MAP) 
	{
std::cout<<"Map process gather .Pidx is "<<m_nPidx<<"\n"<<endl;
		if(!m_isOnline)
			for (int i = 0; i < m_nBlockCnt; i++) 
			{
				BlockParam* tmp = &(m_pBlockParams[i]);
				int root = gid2pid(i, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);
				MPI_Gatherv((void*)m_pMapResultVec[i], tmp->m_nMapEntryLocalNum, m_mpiDataTypeForMap, (void*)nullptr,
					tmp->m_pMapEntryLocalNumVec, tmp->m_pMapEntryDisp, m_mpiDataTypeForMap, root, MPI_COMM_WORLD);
			}
	} 
	else if (m_nGtype == PROCESS_TYPE_REDUCE) 
	{
std::cout<<"Reduce process gather .Pidx is "<<m_nPidx<<"\n"<<endl;
		BlockParam* tmp = &(m_pBlockParams[m_nGidx]);
		int root = m_nPidx;
		safeDelete<Pixel>(m_pMapResultGlobal);
		m_pMapResultGlobal = new Pixel[tmp->m_nMapEntryGlobalNum];
		MPI_Gatherv(NULL, 0, m_mpiDataTypeForMap, (void*)m_pMapResultGlobal,
		tmp->m_pMapEntryLocalNumVec, tmp->m_pMapEntryDisp, m_mpiDataTypeForMap, root, MPI_COMM_WORLD);
	}
}

void ImageStitch::reduceModule() {
	int cntPixel = m_imgOk.cols * m_imgOk.rows;
	Pixelf* resultTemp = new Pixelf[cntPixel];
	ushort* reduceCnt = new ushort[cntPixel];
	m_pReduceResult = (Pixel*)m_imgOk.data;
	memset(resultTemp, 0, cntPixel * sizeof(Pixelf));
	memset(reduceCnt, 0, cntPixel * sizeof(ushort));
	if(m_nGtype == PROCESS_TYPE_REDUCE) 
	{
		MapEntryGlobal *mapTable = m_pBlockParams[m_nGidx].m_pMapTableGlobal;
		for (int idx = 0; idx < m_pBlockParams[m_nGidx].m_nMapEntryGlobalNum; idx++) 
		{
			int mapIdx = mapTable[idx].yGlobal * m_nWidthBlock + mapTable[idx].xGlobal;
			resultTemp[mapIdx] += m_pMapResultGlobal[idx];
			reduceCnt[mapIdx]++;
		}
	} 
	for(int idx = 0; idx < cntPixel; idx++)
		if(reduceCnt[idx] != 0) {
			m_pReduceResult[idx] = (resultTemp[idx] / reduceCnt[idx]);
		} else m_pReduceResult[idx] = 0;
	safeDelete<ushort>(reduceCnt);
	safeDelete<Pixelf>(resultTemp);
}

void ImageStitch::ImageEachScale(IN const int frameIdx) {
	char* filename = new char[m_sDirGlobalImgOut.length() + m_sFileFormatGlobalImgOut.length() + MAX_PATH];
	if(filename == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}

	Mat tmpImg[2];
	tmpImg[0] = m_imgOk.clone();
	int i = 0;
cout<<"*********98 BLOCK have prepared for storing images "<<endl;
cout<<(m_sDirGlobalImgOut + m_sFileFormatGlobalImgOut).c_str()<<0<<m_nGidx<< frameIdx<<endl;;
	for(int ScaleIdx = 0; ScaleIdx < m_nScaleCnt; ScaleIdx++) {
		sprintf(filename, (m_sDirGlobalImgOut + m_sFileFormatGlobalImgOut).c_str(), ScaleIdx, m_nGidx, frameIdx);
		imwrite(filename, tmpImg[i]);
		resize(tmpImg[i], tmpImg[1 - i], Size(tmpImg[i].cols / 2, tmpImg[i].rows / 2),(0,0),(0,0),CV_INTER_AREA);
		i = 1 - i;
	}
	safeDelete<char>(filename);
	return;
}

void ImageStitch::run() {
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
//	MPI_Comm comm_online;
//	MPI_Comm_split(MPI_COMM_WORLD, (m_nGtype == PROCESS_TYPE_COMMUNICATION) || (m_nGtype == PROCESS_TYPE_MAP), 0, &comm_online);
//	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
//		MPI_Comm_rank(comm_online, &onlineRoot);
	MPI_Barrier(MPI_COMM_WORLD);												//		  同步0
//	MPI_Bcast(&onlineRoot, 1, MPI_INT, root, MPI_COMM_WORLD);
	if(m_nGtype == PROCESS_TYPE_REDUCE) {
		while(true) {
//printf("**Reduce  Process communication before the barrier !!! Pidx is  %d \n isRun is %d \n ",m_nPidx,isRun);
//fflush(stdout);
			MPI_Barrier(MPI_COMM_WORLD);                                         //Reduce 同步1
			MPI_Bcast(&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);					//广播1		isRun
			if(!isRun) continue;		
			MPI_Barrier(MPI_COMM_WORLD);										 //Reduce 同步2
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播2		cnt	
			if(cnt < int(sizeof(int))) continue;
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播3		temp
			if(temp[0] == STITCH_OFFLINE) {
				m_isOnline = false;
				while(true) {
				//		printf("**Reduce  Process communication enter the loop of offline . before the barrier !!! Pidx is %d \n isRun is %d \n ",m_nPidx,isRun);
			//fflush(stdout);
			printf("**Redc  Process before the first barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);
				fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Reduce 同步3
					MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);				//广播4		cnt
					MPI_Bcast(temp, 8, MPI_INT, root, MPI_COMM_WORLD);				//广播5		temp
					if(cnt < 2 * sizeof(int) || temp[0] != STITCH_OFFLINE) break;
					m_nFrameIdx = temp[1];
					 
cout<<"Reduce will deal with gatherModule  .    Pidx is "<<m_nPidx<<endl;
fflush(stdout);
					gatherModule();
cout<<"**Reduce have dealt with gatherModule  .    Pidx is "<<m_nPidx<<endl;
fflush(stdout);
					reduceModule();
					ImageEachScale(m_nFrameIdx);
					 
					printf("**Redc  Process before the second barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);	fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Reduce 同步4
				}
			}
		}
	} else if(m_nGtype == PROCESS_TYPE_MAP) {
		while(true) {
			//printf("**Map  Process communication before the barrier !!! Pidx is %d \n isRun is %d \n ",m_nPidx,isRun);
			fflush(stdout);
			MPI_Barrier(MPI_COMM_WORLD);										//Map    同步1
			MPI_Bcast(&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);					//广播1		isRun
			if(!isRun) continue;
			MPI_Barrier(MPI_COMM_WORLD);										//Map	 同步2
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播2		cnt
			if(cnt < sizeof(int)) continue;
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播3		temo
			if(temp[0] == STITCH_OFFLINE) {
				m_isOnline = false;
				while(true) {
				//	printf("**Map  Process communication enter the loop of offline . before the barrier !!! Pidx is %d \n isRun is %d \n ",m_nPidx,isRun);
				//fflush(stdout);
					printf("**Map   Process before the first barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);
				fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Map	 同步3
					MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);				//广播4		cnt
					MPI_Bcast(temp, 8, MPI_INT, root, MPI_COMM_WORLD);				//广播5		temp
					if(cnt < int(2 * sizeof(int))|| temp[0] != STITCH_OFFLINE) break;
					m_nFrameIdx = temp[1];
					 
//cout<<"Map    will deal with LoadImgModule .    Pidx is "<<m_nPidx<<endl;
//fflush(stdout);
					loadImgModule(m_nFrameIdx);
//cout<<"**Map    have dealt with LoadImgModule .    Pidx is "<<m_nPidx<<endl;
//fflush(stdout);
					mapModule();
					gatherModule();
					 
					printf("**Map   Process before the second barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);	fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Map	 同步4
				}
			}
		}
	} else if(m_nGtype == PROCESS_TYPE_COMMUNICATION) {
		struct sockaddr_in remoteAddr;
		int addrLen = sizeof(remoteAddr);
		while(true) {
			printf("Waiting for client of ExcProcess to connect.\n");
			fflush(stdout);
			m_client_exc = accept(m_server, (LPSOCKADDR)&remoteAddr, (socklen_t*)&addrLen);
			if(m_client_exc == INVALID_SOCKET) {
				printf("Process communication fails to accept a connect!\n");
				fflush(stdout);
				isRun = 0;
			} else {
				printf("Process communication accept a connect: %s.!!\n", inet_ntoa(remoteAddr.sin_addr));
				isRun = 1;
				fflush(stdout);
			}
			MPI_Barrier(MPI_COMM_WORLD);									   //Comm	 同步1
			MPI_Bcast((void*)&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);				//广播1		isRun
			if(!isRun) continue;
			cnt = recv(m_client_exc, recvBuf, SOCKET_BUFFER_LENGTH, 0);
cout<<"The first time you received number is "<<cnt<<endl;
cout<<"temp[0]  "<<temp[0]<<" ,temp[1]  "<<temp[1]<<" ,temp[2]  "<<temp[2]<<" ,temp[3]  "<<temp[3]<<" ,temp[4]  "<<temp[4]<<endl;
cout<<"temp[5]  "<<temp[5]<<" ,temp[6]  "<<temp[6]<<" ,temp[7]  "<<temp[7]<<endl;
fflush(stdout);
			MPI_Barrier(MPI_COMM_WORLD);										//Comm	 同步2
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播2		cnt
			if(cnt < sizeof(int)) {closesocket(m_client_exc); continue;}
			MPI_Bcast(temp, 1, MPI_INT, root, MPI_COMM_WORLD);						//广播3		temp
 
			if(temp[0] == STITCH_OFFLINE) {		
				m_isOnline = false;
				while(true) {
					cnt = recv(m_client_exc, recvBuf, SOCKET_BUFFER_LENGTH, 0);
cout<<"The second time you received number is "<<cnt<<endl;
cout<<"temp[0]  "<<temp[0]<<" ,temp[1]  "<<temp[1]<<" ,temp[2]  "<<temp[2]<<" ,temp[3]  "<<temp[3]<<" ,temp[4]  "<<temp[4]<<endl;
cout<<"temp[5]  "<<temp[5]<<" ,temp[6]  "<<temp[6]<<" ,temp[7]  "<<temp[7]<<endl;
			fflush(stdout);
				printf("**Comm  Process before the first barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);
				fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Comm	 同步3
					tic();
					MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);				//广播4		cnt
					MPI_Bcast(temp, 8, MPI_INT, root, MPI_COMM_WORLD);				//广播5     temp
				//	if(cnt < int(2 * sizeof(int)) || temp[0] != STITCH_OFFLINE) break;	
						if(cnt < int(2 * sizeof(int))) break;	
					m_nFrameIdx = temp[1];
						printf("**Comm  Process before the second barrier !!! Pidx is %d , isRun is %d \n ",m_nPidx,isRun);
						fflush(stdout);
					MPI_Barrier(MPI_COMM_WORLD);								//Comm	 同步4
					char sendBuf[] = "ImageStitch offline is finished!\n ";
					toc();
					cout<<sendBuf<<"Finish Time: "<<m_dElapseTime<<" s."<<endl;
					send(m_client_exc, sendBuf, strlen(sendBuf) + 1, 0);
break;
				}
			}
			closesocket(m_client_exc);
		}
		closesocket(m_server);
#ifdef _WINDOWS
		WSACleanup();
#endif
	}
	printf("Process %d finish running.\n", m_nPidx);
	fflush(stdout);
//	MPI_Comm_free(&comm_online);
	return;
}

void ImageStitch::generateStitchParams() {	
	printf("Process %d starts generating scale parameters.\n", m_nPidx);
	fflush(stdout);
	tic();
	char* filename = new char[m_sDirMapTable.length() + MAX_PATH];
	if(filename == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	sprintf(filename, "%s", (m_sDirMapTable + m_sFileFormatMapTable).c_str());

printf("0 Process %d open MapTable %s . \n",m_nPidx,filename);

	FILE* fin = fopen(filename, "rb");
printf("1 Process %d open MapTable %s . \n",m_nPidx,filename);
	if (fin == nullptr) {
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	int fileCnt = m_nCameraCnt + m_nBlockCnt + 1;
	FILE** fout = new FILE*[fileCnt];
	if(fout == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++) {
		int groupType, groupIdx;
		pid2gid(fileIdx, m_nCameraCnt, m_nBlockCnt, groupIdx, groupType);
		sprintf(filename, (m_sDirStitchParam + m_sFileFormatStitchParam).c_str(), groupType, groupIdx);
		fout[fileIdx] = fopen(filename, "wb");
printf("Process %d open 134 %s . \n",m_nPidx,filename);
		if (fout[fileIdx] == nullptr) {
			printf("Process %d could not open file: %s!\n", m_nPidx, filename);
			fflush(stdout);
			terminate(ERROR_OPEN_FILE);
		}
	}

	int cnt;
	fread(&cnt, sizeof(int), 1, fin);
	MapTableEntry* mapTable = new MapTableEntry[cnt];
	if(mapTable == NULL) {
		printf("Process %d could not malloc!\n", m_nPidx);
		fflush(stdout);
		terminate(ERROR_MALLOC);
	}
	fread(mapTable, sizeof(MapTableEntry), cnt, fin);
	fclose(fin);
//
	ofstream File_write;
//	File_write.open("E:/Workspace/VS2010/Aug/OfflineImageStitch/Block_Cam_points.txt",ios::out|ios::binary);
	File_write.open("E:/Library/data/ImageStitch/OfflineImageStitch/StitchParams/Block_Cam_points.txt",ios::out|ios::binary);
 
//

	for (int i = 0; i < m_nBlockRows; i++) {
		for (int j = 0; j < m_nBlockCols; j++) {
			int blockIdx = i * m_nBlockCols + j;
			int xLeftTop = m_nWidthBlock * j, yLeftTop = m_nHeightBlock * i;
			int xRightBottom = min(xLeftTop + m_nWidthBlock - 1, m_nWidthGlobal), yRightBottom = min(yLeftTop + m_nHeightBlock - 1, m_nHeightGlobal);
			int* mapEntryLocalNumVec = new int[m_nCameraCnt];
			if(mapEntryLocalNumVec == NULL) {
				printf("Process %d could not malloc!\n", m_nPidx);
				fflush(stdout);
				terminate(ERROR_MALLOC);
			}
			memset(mapEntryLocalNumVec, 0, m_nCameraCnt * sizeof(int));
			for (int k = 0; k < cnt; k++)
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
				    (mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))
					mapEntryLocalNumVec[mapTable[k].cameraIdx]++;

std::cout<<"The "<<m_nBlockCols*i+j<<"Block arrange 36 Cam\n";
for(int hk=0;hk<36;hk++)
{
	File_write<<mapEntryLocalNumVec[hk]<<" ";
	std::cout<<mapEntryLocalNumVec[hk]<<"  ";

}
File_write<<"\r\n";
std::cout<<"\n";
 fflush(stdout);
 
			for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
			{
				fwrite(mapEntryLocalNumVec, sizeof(int), m_nCameraCnt, fout[fileIdx]);
			 }


			safeDelete<int>(mapEntryLocalNumVec);

			for (int k = 0; k < cnt; k++) {
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
				    (mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom)) {
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

		File_write.close();
	 
	safeDelete<MapTableEntry>(mapTable);
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
		fclose(fout[fileIdx]);
	safeDelete<FILE*>(fout);
	safeDelete<char>(filename);
	toc();
	printf("Process %d finishes generating scale parameters.\n", m_nPidx);
	printf("Time: %f\n", m_dElapseTime);
system("pause");
	fflush(stdout);
	return;
}
