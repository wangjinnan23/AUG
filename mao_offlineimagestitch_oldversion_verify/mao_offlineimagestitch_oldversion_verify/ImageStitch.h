#ifndef IMAGE_STITCH_H

#define IMAGE_STITCH_H
#include"stdafx.h"

typedef int socklen_t;


/// ����ö�����ͣ��������ܵĴ������
enum ErrorType
{
	ERROR_OPEN_FILE           = -1,///< ���ļ�ʧ��
	ERROR_FIND_DIR            = -2,///< Ѱ��·��ʧ��
	ERROR_INVALID_FRAME      
	= -3,///< ��Ч��֡���
	ERROR_MATRIX_INCONTINUOUS = -4,///< ���ٵľ������ڴ�ռ��в�����
	ERROR_SOCKET_WSA_STARTUP  = -5,///< socket��WSA���첽�׽��֣�����ʧ��
	ERROR_SOCKET_SETUP        = -6,///< socket�Ľ���ʧ��
	ERROR_SOCKET_BIND         = -7,///< socket��serverʧ��
	ERROR_SOCKET_LISTEN       = -8,///< socket����ʧ��
};


///����ö�����ͣ��г����ܵĽ��̵�����
enum GroupType
{
	PROCESS_TYPE_NULL			 = -1,///< ���õĽ���
	PROCESS_TYPE_COMMUNICATION   = 0, ///< communication����
	PROCESS_TYPE_MAP			 = 1, ///< map����
	PROCESS_TYPE_REDUCE		     = 2, ///< reduce����
	
	
};

/*********************************************************************//**
* \brief   �����MapEntryLocal�࣬�ֲ�ӳ�����Ŀ���洢��ͼ��ӦСͼ�ֲ�����
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
class MapEntryLocal
{
public:
	float xLocal;
	float yLocal;
	MapEntryLocal(float x = 0,  float y = 0):xLocal(x),yLocal(y) {}
	~MapEntryLocal() {}
};


/*********************************************************************//**
* \brief   �����MapEntryGlobal�࣬ȫ��ӳ�����Ŀ���洢ȫ������
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
class MapEntryGlobal
{
public:
	ushort  xGlobal;
	ushort  yGlobal;
	MapEntryGlobal(ushort x = 0, ushort y = 0):xGlobal(x),yGlobal(y) {};
	~MapEntryGlobal(){}
};

/*********************************************************************//**
* \brief   �����MapTableEntry�࣬ԭʼӳ�����Ŀ
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
**************************************************************************/
class MapTableEntry
{
public:
	ushort		cameraIdx;
	float		xLocal;
	float		yLocal;
	ushort		xGlobal;
	ushort		yGlobal;
	MapTableEntry(ushort idx = 0, float xL = 0, float yL = 0, ushort xG = 0, ushort yG = 0):
	cameraIdx(idx), xLocal(xL), yLocal(yL), xGlobal(xG), yGlobal(yG) {} 
	~MapTableEntry(){}
};

/*********************************************************************//**
* \brief   ���������࣬��Ϊĳһ�ֿ��map��reduce����
* \author  �º���
* \version 1.0b
* \date    2015��7��9��
**************************************************************************/
class BlockParam
{
public:
	/// ���캯��
	BlockParam();
	/// ��������
	~BlockParam();

    int             m_nMapEntryLocalNum;   ///< �ֲ�ӳ�����Ŀ����
	int*            m_pMapEntryLocalNumVec;///< �ֲ�ӳ�����Ŀ��������
	int*            m_pMapEntryDisp;       ///< �ֲ�ӳ����Ӧȫ��ӳ����е�λ������
	int             m_nMapEntryGlobalNum;  ///< ȫ��ӳ�����Ŀ����
	MapEntryLocal*   m_pMapTableLocal;      ///< �ֲ�ӳ���ָ��
	MapEntryGlobal*  m_pMapTableGlobal;     ///< ȫ��ӳ���ָ��

};


/*********************************************************************//**
* \brief   ����ͼ��ƴ���࣬����ͼ����ƴ��
* \author  �º��ȡ���ï��
* \version 1.0b
* \date    2016��4��
* 
*���ӿ�˵����  run()�������ڲ���ͼ��ƴ�ӵ�����
*              generateScaleParams()�����������ɸ��㼶�Ĳ����ļ�
**************************************************************************/
class ImageStitch
{
public:
	/// ���캯��
	ImageStitch();
	/// ��������
	~ImageStitch();
	/// ���к���
	void run();
	/// ���ɸ��㼶�Ĳ����ļ�
	void generateStitchParams();

private:
	static const string FILENAME_PARAMS;///< �����ļ���
/*********************************����**************************************/
	//����MPI�ı���
	int			    m_nPcnt;							///< ���̸���
	int	        	m_nPidx;							///< ���̱��
	char	        m_sPname[MPI_MAX_PROCESSOR_NAME];	///< ����������
	MPI_Datatype	m_mpiDataTypeForMap;				///< ����map���gather��mpi��������
	MPI_Datatype	m_mpiDataTypeForReduce;             ///< ����reduce���gather��mpi��������

	//��������
	int            m_nScaleCnt;              ///< �㼶����
	int            m_nCameraCnt;             ///< ���������
	string         m_sDirLocalImgIn;         ///< ����ľֲ�ͼ������Ŀ¼
	string         m_sFileFormatLocalImgIn;  ///< ����ľֲ�ͼ���ļ�����ʽ
	string         m_sDirGlobalImgOut;       ///< �����ƴ�����ͼ������Ŀ¼
	string         m_sFileFormatGlobalImgOut;///< �����ƴ�����ͼ���ļ�����ʽ
	string         m_sDirMapTable;           ///< ӳ�������Ŀ¼
	string         m_sFileFormatMapTable;    ///< ӳ����ļ�����ʽ
	string		   m_sDirStitchParam;         ///< ƴ�Ӳ����ļ�Ŀ¼
	string		   m_sFileFormatStitchParam;  ///< ƴ�Ӳ����ļ�����ʽ

	

	//���в���
	int				m_nBlockRows;			///<�з���ķֿ���      
	int				m_nBlockCols;			///<�з���ķֿ���
	int			    m_nBlockCnt;			///<�ֿ������
	int				m_nGtype;				///<������������
	int				m_nGidx;			    ///< ������������ı��
	BlockParam*	   m_pBlockParams;			///<�������ָ��


	//���ڼ�ʱ�ı���
	double			m_dStartTime;			///<��¼��ʼʱ��
	double			m_dElapseTime;			///< �ӿ�ʼ��ʱ��ֹͣ��ʱ�����ŵ�ʱ�䣨��λΪs��

	//�м�������
	Mat				m_imgLocal;				///< ��������ֲ�ͼ��
	vector<Pixel*>  m_pMapResultVec;		///< �������map�������
	Pixel*			m_pMapResultGlobal;		///< map����ֿ����
	Pixel*			m_pReduceResult;		///< �ֿ�reduce���
	Pixel*			m_pReduceResultGlobal;	///< reduce����������
	Mat				m_imgOk;				///<ƴ����ɵ�ͼ��
	
	//ƴ�Ӳ�������
	int				m_nFrameBegin; ///<��ʼ֡
	int             m_nFrameCnt;   ///< ֡����
	int				m_nFrameEnd;   ///< ����֡
	int				m_nDataIdx;	   ///< ͼ�����ݱ�ţ�����ʱȡ��dataIdx�궨���ͼ������
	SOCKET			m_server;	   ///<������
	SOCKET			m_client_exc;  ///<��ƴ�ӽ���ͨ�ŵĿͻ��˽���

	///�쳣�˳�����
	void  terminate(IN const int errorCode);

	///��ʼ��ʱ
	void  tic();
	///ֹͣ��ʱ
	void  toc();

	///��ʼ��mpi����
	void  initMPI();

	///��ȡ��������
	void  loadParams();

	///	����ƴ�Ӳ���
	void loadStitchParams();

	///���ò���
	void set(IN const int processCnt, IN const int processIdx, IN const int cameraCnt, IN const int blockRows, IN const int blockCols);
	///��ȡ����
	void load(IN FILE* fin);

	///��ʼ���м����
	void  initInterVar();

	///˫���Բ�ֵ����
	Pixel32  biLinInterp(IN const Mat &value ,IN const float x, IN const float y);

	///map����
	 int   map(IN const Mat &imgLocal, IN const MapEntryLocal* mapTable, IN const int cnt, OUT Pixel* &mapResult);

	///reduce����
	void reduce(IN const Pixel* mapResult, IN const MapEntryGlobal* mapTable, IN const int cntTableEntry, IN const int row, IN const int col, OUT Pixel* &reduceResult);

	///����ͼ��ģ��
	void loadImgModule(IN const int frameIdx = 0);

	///mapģ��
	int mapModule();

	///gatherģ��
	void gatherModule();

	///reduceģ��
	void reduceModule();

	/// �洢ͼ��ģ��
	void saveImgModule(IN const int ScaleIdx , IN const int frameIdx, IN const Mat &Img);

	///����ÿһ�㼶ͼ��
	void ImageEachScale(IN const int frameIdx);

	/// ����ƴ�Ӳ���ģ��
	int recvStitchParams();

	///����ƴ������ź�
	void sendStitchFinish();
};

///�ӽ��̺ŵ����ŵ�ת��
void pid2gid(IN const int pid ,IN const int cameraCnt,IN const int* blockCnt,OUT int &gid, OUT int &gType);
/// �����ŵ����̱�ŵ�ת��
int gid2pid(IN const int gid, IN const int gType, IN const int cameraCnt ,IN const int blockCnt);

#endif