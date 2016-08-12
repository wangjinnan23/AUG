/*********************************************************************//**
* \file      ImageStitch.cpp
* \brief     ͼ��ƴ�����ʵ��
* \author    �º��ȡ���骡���ï��
* \version   1.0b
* \date      2016��4��
* \copyright ���ļ���Ȩ���廪��ѧ����
**************************************************************************/

#include "stdafx.h"
#include "ImageStitch.h"


const string ImageStitch::FILENAME_PARAMS =("E:\\Library\\data\\Verification\\Mao_offlineimageStitch\\ImgStitch.config");

/*********************************************************************//**
* \brief   ���̱�ŵ����ŵ�ת��
* \author  �º���
* \version 1.0b
* \date    2016��4��
*
* \param   pid       ���̱��
* \param   cameraCnt ���������
* \param   blockCnt  ������
* \param   gid       ����
* \param   gType     ������
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
* \brief   ���ŵ����̱�ŵ�ת��
* \author  �º���
* \version 1.0b
* \date    2016��4��
*
* \param   gid       ����
* \param   gType     ������
* \param   cameraCnt ���������
* \param   blockCnt  ������
* \return  ���̱��
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
* \brief   BlockParam��Ĺ��캯��
* \author  �º���
* \version 1.0b
* \date    2016��4��
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
* \brief   BlockParam�����������
* \author  �º���
* \version 1.0b
* \date    2016��4��
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
* \brief   ImageStitch��Ĺ��캯��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
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
* \brief   ImageStitch��Ĺ��캯��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
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
* \brief   �쳣�˳�
* \author  �º���
* \version 1.0b
* \date    2016��4��
*
* \param   errorCode �������
*
* \sa      ErrorType
**************************************************************************/
void ImageStitch::terminate(IN const int errorCode)
{
	fflush(stdout);
	MPI_Abort(MPI_COMM_WORLD, errorCode);
}


/*********************************************************************//**
* \brief   ��ʼ��ʱ
* \author  �º���
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::tic()
{
	m_dStartTime = MPI_Wtime();
	return;
}

/*********************************************************************//**
* \brief   ֹͣ��ʱ����¼����ʱ��
* \author  �º���
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::toc()
{
	m_dElapseTime = MPI_Wtime() - m_dStartTime;
	return;
}


/*********************************************************************//**
* \brief   ��ʼ��mpi�������
* \author  �º���
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::initMPI()
{
	// �õ��������������̱�źʹ���������
	MPI_Comm_size(MPI_COMM_WORLD, &m_nPcnt);
	MPI_Comm_rank(MPI_COMM_WORLD, &m_nPidx);
	
//	m_nPidx = 60;
	
	int nameLen;
	MPI_Get_processor_name(m_sPname, &nameLen);
	printf("Process %d of %d is on %s.\n", m_nPidx, m_nPcnt, m_sPname);
	fflush(stdout);

	// ����mpi������������mpiͨ��
	m_mpiDataTypeForMap    = MPI_UNSIGNED_SHORT;
	m_mpiDataTypeForReduce = MPI_UNSIGNED_SHORT;

	return;
}

/*********************************************************************//**
* \brief   ��ȡ��������
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \sa      pid2gid()
**************************************************************************/
void ImageStitch::loadParams()
{
	// �򿪲����ļ�
	FileStorage fs(FILENAME_PARAMS, FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("Process %d could not open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}
		printf("Process %d open parameter file: %s!\n", m_nPidx, FILENAME_PARAMS.c_str());
	// ����㼶���������������
	fs["ScaleCnt"]  >> m_nScaleCnt;
	fs["CamaraCnt"] >> m_nCameraCnt;

	// ����ӳ�������Ŀ¼��ӳ����ļ�����ʽ���㼶��������Ŀ¼���㼶�����ļ�����ʽ
	fs["DirMapTable"]				 >> m_sDirMapTable;
	fs["FileFormatMapTable"]		 >> m_sFileFormatMapTable;
	fs["DirStitchParam"]			 >> m_sDirStitchParam;
	fs["FileFormatStitchParam"]   >> m_sFileFormatStitchParam;

			
	cout<<m_sDirMapTable<<endl;
	cout<<m_sDirStitchParam<<endl;

	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;
	// ���µ�ǰ��������������ͼ����
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);

	int flag;
	if (m_nGtype == PROCESS_TYPE_MAP)// map����
	{
		// ��������ľֲ�ͼ������Ŀ¼������ľֲ�ͼ���ļ�����ʽ
		fs["DirLocalImgIn"]        >> m_sDirLocalImgIn;
		fs["FileFormatLocalImgIn"] >> m_sFileFormatLocalImgIn;
	}

	if (m_nGtype == PROCESS_TYPE_REDUCE)// communication����
	{
		// ���������ȫ��ͼ������Ŀ¼�������ȫ��ͼ���ļ�����ʽ
		fs["DirGlobalImgOut"]        >> m_sDirGlobalImgOut;
		fs["FileFormatGlobalImgOut"] >> m_sFileFormatGlobalImgOut;

				// ���������Ŀ¼���ж��Ƿ�ɹ������ʧ�����˳�
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
* \brief   ����ƴ�Ӳ���
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \sa      pid2gid(), ImageStitch::set(), ImageStitch::load()
**************************************************************************/
void ImageStitch::loadStitchParams()
{

	// ���µ�ǰ���̵������ͼ����
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);

	// �򿪱����̶�Ӧ�Ĳ����ļ�����ʧ�����˳�
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

	// �Ըò㼶�Ĳ����������ú����룬Ȼ��ر��ļ�
	set(m_nPcnt, m_nPidx, m_nCameraCnt, m_nBlockRows, m_nBlockCols);
	load(fin);
	fclose(fin);
	safeDelete<char>(filename);
	return;
}

/*********************************************************************//**
* \brief   ImageStitch�����ò�����
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   processCnt ��������
* \param   processIdx ���̱��
* \param   camaraCnt  ���������
* \param   blockRows  �з���Ŀ�����
* \param   blockCols  �з���Ŀ�����
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
* \brief   ImageStitch������뺯��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   fin �����ļ�ָ��
**************************************************************************/
void ImageStitch::load(IN FILE* fin)
{
	// ��������������ռ�
	m_pBlockParams = new BlockParam[m_nBlockCnt];
	if(m_pBlockParams == NULL)
	{
		return;
	}
	for (int i = 0; i < m_nBlockCnt; i++)// �������д���
	{
		BlockParam* blockParam = &(m_pBlockParams[i]);
		if (m_nGtype == PROCESS_TYPE_MAP || m_nGtype == PROCESS_TYPE_REDUCE)// map��reduce����
		{
			// ���롰�ֲ�ӳ�����Ŀ�������顱
			blockParam->m_pMapEntryLocalNumVec = new int[m_nPcnt];
			if(blockParam->m_pMapEntryLocalNumVec == NULL)
			{
				return;
			}
			memset(blockParam->m_pMapEntryLocalNumVec, 0, m_nPcnt * sizeof(int));
			for (int j = 0; j < m_nCameraCnt; j++)// �ļ���ֻ�洢��Ӧÿ�����������Ŀ��������Ϊ�����Ĳ�û��
			{    //ÿ���鶼�����Ӧÿ�����������Ŀ����
				int temp = gid2pid(j, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);// �˴������������ŵ����̱�ŵ�ӳ��
				fread(&((blockParam->m_pMapEntryLocalNumVec)[temp]), sizeof(int),1, fin);
			}

			// ���ݡ��ֲ�ӳ�����Ŀ�������顱���㡰�ֲ�ӳ����Ӧȫ��ӳ����е�λ�����顱
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

			// ����Ŀ��Ϊ��ȫ��ӳ�����Ŀ������
			blockParam->m_nMapEntryGlobalNum = (blockParam->m_pMapEntryDisp)[m_nPcnt - 1] + (blockParam->m_pMapEntryLocalNumVec)[m_nPcnt - 1];

		}

		if (m_nGtype == PROCESS_TYPE_MAP)// map����
		{
			blockParam->m_nMapEntryLocalNum = (blockParam->m_pMapEntryLocalNumVec)[m_nPidx];
			if (blockParam->m_nMapEntryLocalNum > 0)// ӳ�䵽��ǰ�����Ŀ��Ϊ0ʱ�Ŷ�ȡ�ֲ�ӳ���
			{
				blockParam->m_pMapTableLocal = new MapEntryLocal[blockParam->m_nMapEntryLocalNum];
				if(blockParam->m_pMapTableLocal == NULL)
				{
					return;
				}
				fread(blockParam->m_pMapTableLocal, sizeof(MapEntryLocal), blockParam->m_nMapEntryLocalNum, fin);
			}
		}

		if (m_nGtype == PROCESS_TYPE_REDUCE)// reduce����
		{
			if (i == m_nGidx)// ֻ�п�����뵱ǰreduce���̵Ŀ����һ��ʱ�Ŷ�ȡȫ��ӳ���
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
* \brief   ��ʼ���м����
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
* \sa      pid2gid(), GroupType
**************************************************************************/
void  ImageStitch::initInterVar()
{
	// ������ز����ĸ���
	pid2gid(m_nPidx, m_nCameraCnt, m_nBlockCnt, m_nGidx, m_nGtype);
	if(m_nGtype == PROCESS_TYPE_COMMUNICATION)
	{
//ifdef _WINDOWS
		// �����׽��ַ���
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

		// ����server socket
		m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(m_server == INVALID_SOCKET)
		{
			printf("Process communication could not set up socket.\n");
			fflush(stdout);
			terminate(ERROR_SOCKET_SETUP);
		}
		printf("Process communication set up socket successfully.\n");
		fflush(stdout);
		
		//��server
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

		//��ʼ����
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
* \brief   ˫���Բ�ֵ
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   value ֵ����
* \param   x     X����
* \param   y     Y����
*
* \return  ��ֵ���
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

	if (i == x)// x������������x������Ҫ��ֵ
	{
		vE = vA;
		if (j == y)// y������������vֱ�Ӹ�ֵΪvE
		{
			v.datatmp  = vE;
		}
		else// y���겻����������ȡvF����ֵ
		{
			vF = (float)value.at<uchar>(j + 1, i);
//			vF = value.at<float>(j + 1, i);
			v.datatmp  = (y - j) * (vF - vE) + vE;
		}
	}
	else// x���겻��������x�����Ȳ�ֵ
	{
//		vB = value.at<float>(j, i + 1);
		vB = (float)value.at<uchar>(j, i + 1);
		vE = (x - i) * (vB - vA) + vA;
		if (j == y)// y������������vֱ�Ӹ�ֵΪvE
		{
			v.datatmp  = vE;
		}
		else// y���겻����������ȡvC��vD����ֵ��vF���ٲ�ֵ��v
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
* \brief   map����
* \author  �º��ȡ���骡���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   imgLocal  �ֲ�ͼ��
* \param   mapTable  �ֲ�ӳ���
* \param   cnt       �ֲ�ӳ�����
* \param   mapResult map���
*
* \return  map�������
*
* \sa      biLinInterp()
**************************************************************************/
int ImageStitch::map(IN const Mat &imgLocal, IN const MapEntryLocal* mapTable, IN const int cnt, OUT Pixel* &mapResult)
{
	// ���mapResult�Ƿ�Ϊ��ָ�룬�������ͷ��ڴ沢��ֵΪ��ָ��
	if (mapResult != nullptr)
	{
		delete []mapResult;
		mapResult = nullptr;
	}

	// ӳ�����Ŀ��Ϊ0��ֱ�ӷ���
	if (cnt == 0)
	{
		return(0);
	}
	//printf("Process %d is in map.\n",m_nPidx);

	// �������˫���Բ�ֵӳ��
	mapResult = new Pixel[cnt];
	for (int idx = 0; idx < cnt; idx++)
	{
		mapResult[idx].Pixel32to16( biLinInterp(imgLocal, mapTable[idx].xLocal, mapTable[idx].yLocal));

	}
	///

	// ����mapResult�����鳤�ȣ�����ӳ���ĳ���
	return(cnt);
}




/*********************************************************************//**
* \brief   reduce����
* \author  �º��ȡ���骡���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   mapResult     map���
* \param   mapTable      ȫ��ӳ���
* \param   cntTableEntry ȫ��ӳ�����
* \param   row           �ֿ�ͼ������
* \param   col           �ֿ�ͼ������
* \param   reduceResult  �ֿ�reduce���
**************************************************************************/
void ImageStitch::reduce(IN const Pixel* mapResult, IN const MapEntryGlobal* mapTable, IN const int cntTableEntry, IN const int row, IN const int col, OUT Pixel* &reduceResult)
{
	// ���reduceResult�Ƿ�Ϊ��ָ�룬�������ͷ��ڴ沢��ֵΪ��ָ��
	if (reduceResult != nullptr)
	{
		delete []reduceResult;
		reduceResult = nullptr;
	}

	// Ϊ������м�������ռ�
	int cntPixel = row * col;//ĳһ�����ڴ�ͼ�ܵ���
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

	// ������мӷ���ע��ѭ������ΪcntTableEntry������cntPixel  ��cntTableEntry��ȫ��ӳ���ĳ���
	for (int idx = 0; idx < cntTableEntry; idx++)
	{
		resultTemp[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal]= resultTemp[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal] + mapResult[idx];
		//mapTable[idx].yGlobal * col + mapTable[idx].xGlobal�ǰѶ�ά�����ת����һά�����ж�Ӧλ��
		reduceCnt[mapTable[idx].yGlobal * col + mapTable[idx].xGlobal]++;//��Ӧ�㴦��������Ŀ��1
	}


	// ������г�����ע��ѭ������ΪcntPixel������cntTableEntry
	for (int idx = 0; idx < cntPixel; idx++)
	{
		if (reduceCnt[idx] != 0)// ע�����������0���������ֳ�0�Ĵ���
		{
			reduceResult[idx].Pixel32to16((resultTemp[idx] / reduceCnt[idx]));//��ƽ��ֵ������Լ
		}
	}

	safeDelete<ushort>(reduceCnt);
	safeDelete<Pixel32>(resultTemp);
	return;
}

/*********************************************************************//**
* \brief   ����ͼ��ģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   frameIdx ֡���
**************************************************************************/
void ImageStitch::loadImgModule(IN const int frameIdx)
{
	if (frameIdx < m_nFrameBegin || frameIdx > m_nFrameEnd)// ֡�����Ч���쳣�˳�
	{
		printf("Process %d tempts to read image file of invalid frame %d!\n", m_nPidx, frameIdx);
		fflush(stdout);
		terminate(ERROR_INVALID_FRAME);
	}

	// �����ļ���������ͼ���ж��Ƿ�Ϊ�գ����Ϊ�����쳣�˳�
	char* filename = new char[m_sDirLocalImgIn.length() + 260];
	if(filename == NULL)
	{
		return;
	}
	int CIdx = m_nGidx +1;  //����Ϊ0����cameraCnt-1����������ͼ��Ƭʱ������1-cameraCnt

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

	// ת��Ϊ����������
	//m_imgLocal.convertTo(m_imgLocal, CV_32FC1);
	return;
}

/*********************************************************************//**
* \brief   mapģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \sa      map()
**************************************************************************/
int ImageStitch::mapModule()
{
	if (!m_pMapResultVec.empty())// �������map�������ǿգ������ͷ��ڴ棬Ȼ�����
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
	m_pMapResultVec.assign(m_nBlockCnt, (Pixel*)nullptr);// ���䳤�Ȳ�ȫ����ֵΪ��ָ��

	for (int i = 0; i < m_nBlockCnt; i++)// ������Ҫ�Ŀ�������map����
	{
	
		BlockParam* tmp = &(m_pBlockParams[i]);// ��ȡ��Ӧ�Ŀ����
		map(m_imgLocal, tmp->m_pMapTableLocal, tmp->m_nMapEntryLocalNum, m_pMapResultVec[i]);// ����map����
//		printf("Process %d  m_nMapEntryGlobalNum is :%d",m_nPidx,tmp->m_pMapTableLocal);
	}

	return(1);
}

/*********************************************************************//**
* \brief   gatherģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::gatherModule()
{
	if (m_nGtype == PROCESS_TYPE_MAP)// map����
	{

		for (int i = 0; i < m_nBlockCnt; i++)// ������gather
		{
			BlockParam* tmp = &(m_pBlockParams[i]);
			int root = gid2pid(i, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);//��rootΪ��ǰ��Ŷ�Ӧ�Ľ��̺ţ���reduceģ�飬tmp->m_nMapEntryLocalNum�п���Ϊ0��Ϊ0��ʾ��map���̲���������
			MPI_Gatherv((void*)m_pMapResultVec[i], tmp->m_nMapEntryLocalNum, m_mpiDataTypeForMap, (void*)nullptr,
				tmp->m_pMapEntryLocalNumVec, tmp->m_pMapEntryDisp, m_mpiDataTypeForMap, root, MPI_COMM_WORLD);
		}
	}
	else if (m_nGtype == PROCESS_TYPE_REDUCE)// reduce����
	{
			BlockParam* tmp = &(m_pBlockParams[m_nGidx]);
			int root = m_nPidx;//��ǰ�Ľ��̺ţ�����ǰreduce���̽����ռ�
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
* \brief   reduceģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \sa      reduce()
**************************************************************************/
void ImageStitch::reduceModule()
{
		BlockParam* tmp = &(m_pBlockParams[m_nGidx]);
		int row = m_nHeightBlock;// ��ȡ�÷ֿ������
		int col = m_nWidthBlock;// ��ȡ�÷ֿ������
		reduce(m_pMapResultGlobal, tmp->m_pMapTableGlobal, tmp->m_nMapEntryGlobalNum, row, col, m_pReduceResult);// ����reduce����

	return;
}

/*********************************************************************//**
* \brief   �洢ͼ��ģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
*
* \param   ScaleIdx �㼶��� frameIdx ֡���
**************************************************************************/
void ImageStitch::saveImgModule(IN const int ScaleIdx,IN const int frameIdx , IN const Mat &Img )
{
	if (frameIdx <= 0 || frameIdx > m_nFrameCnt)// ֡�����Ч���쳣�˳�
	{
		printf("Process %d tempts to save image file of invalid frame %d!\n", m_nPidx, frameIdx);
		fflush(stdout);
		terminate(ERROR_INVALID_FRAME);
	}
	//����ͼ��洢·��
	char* filename = new char[m_sDirGlobalImgOut.length() + 260];
	if(filename == NULL)
	{
		return;
	}
	sprintf(filename, (m_sDirGlobalImgOut + m_sFileFormatGlobalImgOut).c_str(), ScaleIdx, m_nGidx, frameIdx);

//	printf("out file name:%s\n", filename);
//	fflush(stdout);
	
	imwrite(filename, Img);// �洢ͼ��
	return;
}


/*********************************************************************//**
* \brief   ����ÿһ�㼶��ͼ��
* \author  ��ï��
* \version 1.0b
* \date    2016��4��
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
    m_imgOk = Mat(m_nHeightBlock, m_nWidthBlock,CV_16UC1,tmp);//������߷ֱ��ʵĿ�
	m_imgOk.convertTo(m_imgOk, CV_8UC1);


	Mat src = m_imgOk.clone();
	Mat dst;
	for(int idx = 0; idx < m_nScaleCnt; idx++)  //����ÿһ�㼶�ķֿ�ͼ�񲢱���
	{
		saveImgModule(idx, frameIdx, src);
		resize(src, dst, Size(src.cols / 2, src.rows / 2),(0,0),(0,0),CV_INTER_AREA);
		src = dst;
	}

	safeDelete<ushort>(tmp);
	return;
}



/*********************************************************************//**
* \brief   ����ƴ�Ӳ���ģ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
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
		m_nFrameBegin  = atoi(recvBuf);   //��ʼ֡
		m_nFrameCnt	   = atoi(&recvBuf[2]);   //��Ҫƴ�ӵ�֡����
		m_nDataIdx	   = atoi(&recvBuf[4]);   //֪ͨ����ͼ��ģ��ȡ���Ϊm_nDataIdx�ļ������ͼ������
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
* \brief   ����ƴ�������Ϣ
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::sendStitchFinish()
{
	char sendBuf[256] = "ImageStitch offline is finished!\n ";
	cout<<sendBuf<<endl;
	send(m_client_exc, sendBuf, 256, 0);
}



/*********************************************************************//**
* \brief   ���к���
* \author  �º��� ����ï��
* \version 1.0b
* \date    2016��4��
*
* \sa      loadScaleParams(), initInterVar()
**************************************************************************/
void ImageStitch::run()
{
	
	// ������㼶����
	loadStitchParams();
	printf("Process %d finish image stitch params loading.\n", m_nPidx);
	fflush(stdout);

	// ��ʼ���м����
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
				isRun = 0;//isRun���Ϊ0��ѭ�������������ȴ���������
			}
			else//����������ܳɹ������ip��ַ��isRun���Ϊ1
			{
				printf("Process communication accept a connect: %s.\n", inet_ntoa(remoteAddr.sin_addr));
				fflush(stdout);
				isRun = 1;
			}
		}

		// �㲥isRun����������ֵ�����Ƿ������һ��
		MPI_Bcast((void*)&isRun, 1, MPI_INT, root, MPI_COMM_WORLD);
		if(isRun == 0)
		{
			continue;
		}
	

		MPI_Barrier(MPI_COMM_WORLD);
	
		int flag = recvStitchParams();// ����ʾ�ͻ��˽��ղ�������ʼ�ͽ���֡���Ƿ�ǿ��ֹͣ��־��
		if(flag == -1)// ���ղ������ݣ�˵��client�ѶϿ�����,���µȴ�����
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
		else // ���ղ����ɹ�
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
				else if (m_nGtype == PROCESS_TYPE_REDUCE) //��������ͼ�񣬴洢�ڱ���
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
		// ����ƴ����ɣ�communication���̽��ÿͻ��˶�Ӧ��socket�رգ��ٴ�ѭ�����ȴ���һ��ƴ������
		if (m_nGtype == PROCESS_TYPE_COMMUNICATION)
		{
				//�����ͻ���ƴ�������Ϣ
		      sendStitchFinish();
		      closesocket(m_client_exc);
		}
	}
	// ��ѭ�����˳�����رշ���˶�Ӧ��socket��������׽��ַ���
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
* \brief   ���ɸ��㼶�Ĳ����ļ�
* \author  �º���
* \version 1.0b
* \date    2016��4��
**************************************************************************/
void ImageStitch::generateStitchParams()
{	
	printf("Process %d starts generating scale parameters.\n", m_nPidx);
	fflush(stdout);

	char* filename = new char[m_sDirMapTable.length() + 260];//�����ļ���ָ��
	if(filename == NULL)
	{
		return;
	}
//#ifdef _WINDOWS
	_mkdir(m_sDirStitchParam.c_str());
//#else
//	mkdir(m_sDirScaleParam.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//#endif
	sprintf(filename, (m_sDirMapTable + m_sFileFormatMapTable).c_str());//���ļ���Ŀ¼д��filename�У���������һ�����ļ�
	FILE* fin = fopen(filename, "rb");//���ļ�filename���Զ�������ʽ��ȡ
	if (fin == nullptr)//�ж��Ƿ�Ϊ��ָ��
	{
		printf("Process %d could not open file: %s!\n", m_nPidx, filename);
		fflush(stdout);
		terminate(ERROR_OPEN_FILE);
	}

	m_nBlockRows = (m_nHeightGlobal % m_nHeightBlock == 0) ? (m_nHeightGlobal / m_nHeightBlock) : (m_nHeightGlobal / m_nHeightBlock + 1);
	m_nBlockCols = (m_nWidthGlobal % m_nWidthBlock == 0) ? (m_nWidthGlobal / m_nWidthBlock) : (m_nWidthGlobal / m_nWidthBlock + 1);
	m_nBlockCnt = m_nBlockRows * m_nBlockCols;//�ֿ������Ŀ
	int fileCnt = m_nCameraCnt + m_nBlockCnt + 1;//�ļ���Ŀ
	FILE** fout = new FILE*[fileCnt];
	if(fout == NULL)
	{
		return;
	}
	for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)//fileIdx��ʾ��idx���ļ�
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

	int cnt;//��ʾԭʼӳ�����ĿMapTableEntry����Ŀ
	fread(&cnt, sizeof(int), 1, fin);
	MapTableEntry* mapTable = new MapTableEntry[cnt];  //maptable��ʾȫ��ӳ���
	if(mapTable == NULL)
	{
		return;
	}
	fread(mapTable, sizeof(MapTableEntry), cnt, fin);//���ļ��ж���ԭʼӳ���MapTable
	fclose(fin);

	for (int i = 0; i < m_nBlockRows; i++)  //�з���ķֿ���m_nBlockRows
	{
		for (int j = 0; j < m_nBlockCols; j++)
		{
			int blockIdx = i * m_nBlockCols + j;//
			int xLeftTop = m_nWidthBlock * j, yLeftTop = m_nHeightBlock * i;//��j�����Ͻ�����
			int xRightBottom = min(xLeftTop + m_nWidthBlock - 1, m_nWidthGlobal), yRightBottom = min(yLeftTop + m_nHeightBlock - 1, m_nHeightGlobal);
			//���½�����
			// ��¼��ǰ���ÿ��������ռ�����������
			int* mapEntryLocalNumVec = new int[m_nCameraCnt];
			if(mapEntryLocalNumVec == NULL)
			{
				return;
			}
			memset(mapEntryLocalNumVec, 0, m_nCameraCnt * sizeof(int));
			for (int k = 0; k < cnt; k++)
			{
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
					(mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))//�ж������Ƿ�������Ӧ�Ŀ���
				{
					mapEntryLocalNumVec[mapTable[k].cameraIdx]++;//�ڸÿ������������ռ�����������+1
				}
			}
			for (int fileIdx = 0; fileIdx < fileCnt; fileIdx++)
			 {
				int groupType, groupIdx;
				pid2gid(fileIdx, m_nCameraCnt, m_nBlockCnt, groupIdx, groupType);
				if (groupType == PROCESS_TYPE_MAP || groupType == PROCESS_TYPE_REDUCE)
				{
					fwrite(mapEntryLocalNumVec, sizeof(int), m_nCameraCnt, fout[fileIdx]);//���ռ�����ÿ���������������Ŀд���Ӧ���ļ�
				}
			 }

			safeDelete<int>(mapEntryLocalNumVec);

			for (int k = 0; k < cnt; k++)//��˳��������еĵ�
			{
				if ((mapTable[k].xGlobal >= xLeftTop && mapTable[k].xGlobal <= xRightBottom) &&
					(mapTable[k].yGlobal >= yLeftTop && mapTable[k].yGlobal <= yRightBottom))//�ж������Ƿ�������Ӧ�Ŀ���
				{
					int fileIdx = gid2pid(mapTable[k].cameraIdx, PROCESS_TYPE_MAP, m_nCameraCnt, m_nBlockCnt);
					MapEntryLocal tmpLocal(mapTable[k].xLocal, mapTable[k].yLocal);
					fwrite(&tmpLocal, sizeof(MapEntryLocal), 1, fout[fileIdx]);//Mapֻ��Ҫд��ֲ����꣬��Ϊֻ�õ��ľֲ�����
					fileIdx = gid2pid(blockIdx, PROCESS_TYPE_REDUCE, m_nCameraCnt, m_nBlockCnt);
					MapEntryGlobal tmpGlobal(mapTable[k].xGlobal - xLeftTop, mapTable[k].yGlobal - yLeftTop);
					fwrite(&tmpGlobal, sizeof(MapEntryGlobal), 1, fout[fileIdx]);//reduceֻ�õ���ȫ�����ֻ꣬д��ȫ������
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