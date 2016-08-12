#ifndef IMAGE_STITCH_H

#define IMAGE_STITCH_H
#include"stdafx.h"

typedef int socklen_t;


/// 定义枚举类型，给出可能的错误代码
enum ErrorType
{
	ERROR_OPEN_FILE           = -1,///< 打开文件失败
	ERROR_FIND_DIR            = -2,///< 寻找路径失败
	ERROR_INVALID_FRAME      
	= -3,///< 无效的帧序号
	ERROR_MATRIX_INCONTINUOUS = -4,///< 开辟的矩阵在内存空间中不连续
	ERROR_SOCKET_WSA_STARTUP  = -5,///< socket的WSA（异步套接字）启动失败
	ERROR_SOCKET_SETUP        = -6,///< socket的建立失败
	ERROR_SOCKET_BIND         = -7,///< socket绑定server失败
	ERROR_SOCKET_LISTEN       = -8,///< socket监听失败
};


///定义枚举类型，列出可能的进程的类型
enum GroupType
{
	PROCESS_TYPE_NULL			 = -1,///< 无用的进程
	PROCESS_TYPE_COMMUNICATION   = 0, ///< communication进程
	PROCESS_TYPE_MAP			 = 1, ///< map进程
	PROCESS_TYPE_REDUCE		     = 2, ///< reduce进程
	
	
};

/*********************************************************************//**
* \brief   定义块MapEntryLocal类，局部映射表条目，存储大图对应小图局部坐标
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
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
* \brief   定义块MapEntryGlobal类，全局映射表条目，存储全局坐标
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
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
* \brief   定义块MapTableEntry类，原始映射表条目
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
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
* \brief   定义块参数类，作为某一分块的map和reduce参数
* \author  陈宏钊
* \version 1.0b
* \date    2015年7月9日
**************************************************************************/
class BlockParam
{
public:
	/// 构造函数
	BlockParam();
	/// 析构函数
	~BlockParam();

    int             m_nMapEntryLocalNum;   ///< 局部映射表条目数量
	int*            m_pMapEntryLocalNumVec;///< 局部映射表条目数量数组
	int*            m_pMapEntryDisp;       ///< 局部映射表对应全局映射表中的位置数组
	int             m_nMapEntryGlobalNum;  ///< 全局映射表条目数量
	MapEntryLocal*   m_pMapTableLocal;      ///< 局部映射表指针
	MapEntryGlobal*  m_pMapTableGlobal;     ///< 全局映射表指针

};


/*********************************************************************//**
* \brief   定义图像拼接类，用于图像并行拼接
* \author  陈宏钊、周茂民
* \version 1.0b
* \date    2016年4月
* 
*【接口说明】  run()函数用于并行图像拼接的运行
*              generateScaleParams()函数用于生成各层级的参数文件
**************************************************************************/
class ImageStitch
{
public:
	/// 构造函数
	ImageStitch();
	/// 析构函数
	~ImageStitch();
	/// 运行函数
	void run();
	/// 生成各层级的参数文件
	void generateStitchParams();

private:
	static const string FILENAME_PARAMS;///< 参数文件名
/*********************************变量**************************************/
	//用于MPI的变量
	int			    m_nPcnt;							///< 进程个数
	int	        	m_nPidx;							///< 进程编号
	char	        m_sPname[MPI_MAX_PROCESSOR_NAME];	///< 处理器名称
	MPI_Datatype	m_mpiDataTypeForMap;				///< 用于map结果gather的mpi数据类型
	MPI_Datatype	m_mpiDataTypeForReduce;             ///< 用于reduce结果gather的mpi数据类型

	//给定参数
	int            m_nScaleCnt;              ///< 层级数量
	int            m_nCameraCnt;             ///< 像感器数量
	string         m_sDirLocalImgIn;         ///< 输入的局部图像所在目录
	string         m_sFileFormatLocalImgIn;  ///< 输入的局部图像文件名格式
	string         m_sDirGlobalImgOut;       ///< 输出的拼接完成图像所在目录
	string         m_sFileFormatGlobalImgOut;///< 输出的拼接完成图像文件名格式
	string         m_sDirMapTable;           ///< 映射表所在目录
	string         m_sFileFormatMapTable;    ///< 映射表文件名格式
	string		   m_sDirStitchParam;         ///< 拼接参数文件目录
	string		   m_sFileFormatStitchParam;  ///< 拼接参数文件名格式

	

	//运行参数
	int				m_nBlockRows;			///<行方向的分块数      
	int				m_nBlockCols;			///<列方向的分块数
	int			    m_nBlockCnt;			///<分块的数量
	int				m_nGtype;				///<进程所属类型
	int				m_nGidx;			    ///< 进程在所在组的编号
	BlockParam*	   m_pBlockParams;			///<块参数类指针


	//用于计时的变量
	double			m_dStartTime;			///<记录开始时间
	double			m_dElapseTime;			///< 从开始计时到停止计时所流逝的时间（单位为s）

	//中间结果变量
	Mat				m_imgLocal;				///< 单像感器局部图像
	vector<Pixel*>  m_pMapResultVec;		///< 单像感器map结果数组
	Pixel*			m_pMapResultGlobal;		///< map结果分块汇总
	Pixel*			m_pReduceResult;		///< 分块reduce结果
	Pixel*			m_pReduceResultGlobal;	///< reduce结果整体汇总
	Mat				m_imgOk;				///<拼接完成的图像
	
	//拼接参数变量
	int				m_nFrameBegin; ///<起始帧
	int             m_nFrameCnt;   ///< 帧数量
	int				m_nFrameEnd;   ///< 结束帧
	int				m_nDataIdx;	   ///< 图像数据编号，载入时取第dataIdx标定后的图像数据
	SOCKET			m_server;	   ///<服务器
	SOCKET			m_client_exc;  ///<与拼接进程通信的客户端进程

	///异常退出函数
	void  terminate(IN const int errorCode);

	///开始计时
	void  tic();
	///停止计时
	void  toc();

	///初始化mpi内容
	void  initMPI();

	///读取给定参数
	void  loadParams();

	///	载入拼接参数
	void loadStitchParams();

	///设置参数
	void set(IN const int processCnt, IN const int processIdx, IN const int cameraCnt, IN const int blockRows, IN const int blockCols);
	///读取函数
	void load(IN FILE* fin);

	///初始化中间变量
	void  initInterVar();

	///双线性插值函数
	Pixel32  biLinInterp(IN const Mat &value ,IN const float x, IN const float y);

	///map函数
	 int   map(IN const Mat &imgLocal, IN const MapEntryLocal* mapTable, IN const int cnt, OUT Pixel* &mapResult);

	///reduce函数
	void reduce(IN const Pixel* mapResult, IN const MapEntryGlobal* mapTable, IN const int cntTableEntry, IN const int row, IN const int col, OUT Pixel* &reduceResult);

	///载入图像模块
	void loadImgModule(IN const int frameIdx = 0);

	///map模块
	int mapModule();

	///gather模块
	void gatherModule();

	///reduce模块
	void reduceModule();

	/// 存储图像模块
	void saveImgModule(IN const int ScaleIdx , IN const int frameIdx, IN const Mat &Img);

	///生成每一层级图像
	void ImageEachScale(IN const int frameIdx);

	/// 接收拼接参数模块
	int recvStitchParams();

	///发送拼接完成信号
	void sendStitchFinish();
};

///从进程号到组编号的转换
void pid2gid(IN const int pid ,IN const int cameraCnt,IN const int* blockCnt,OUT int &gid, OUT int &gType);
/// 从组编号到进程编号的转换
int gid2pid(IN const int gid, IN const int gType, IN const int cameraCnt ,IN const int blockCnt);

#endif