#ifndef IMAGE_STITCH_H
#define IMAGE_STITCH_H

#include "stdafx.h"
#include "Pixel.h"
typedef int socklen_t;

enum ErrorType {
	ERROR_OPEN_FILE           = -1,
	ERROR_FIND_DIR            = -2,
	ERROR_INVALID_FRAME       = -3,
	ERROR_MATRIX_INCONTINUOUS = -4,
	ERROR_SOCKET_WSA_STARTUP  = -5,
	ERROR_SOCKET_SETUP        = -6,
	ERROR_SOCKET_BIND         = -7,
	ERROR_SOCKET_LISTEN       = -8,
	ERROR_MALLOC              = -9,
};

enum GroupType {
	PROCESS_TYPE_NULL            =-1,
	PROCESS_TYPE_COMMUNICATION   = 0,
	PROCESS_TYPE_MAP             = 1,
	PROCESS_TYPE_REDUCE          = 2
};

class MapEntryLocal {
 public:
	float xLocal;
	float yLocal;
	MapEntryLocal(float x = 0,  float y = 0):xLocal(x),yLocal(y) {}
	~MapEntryLocal() {}
};
class MapEntryGlobal {
 public:
	ushort  xGlobal;
	ushort  yGlobal;
	MapEntryGlobal(ushort x = 0, ushort y = 0):xGlobal(x),yGlobal(y) {};
	~MapEntryGlobal(){}
};
class MapTableEntry {
 public:
	ushort  cameraIdx;
	float   xLocal;
	float   yLocal;
	ushort  xGlobal;
	ushort  yGlobal;
	MapTableEntry(ushort idx = 0, float xL = 0, float yL = 0, ushort xG = 0, ushort yG = 0):
	              cameraIdx(idx), xLocal(xL), yLocal(yL), xGlobal(xG), yGlobal(yG) {} 
	~MapTableEntry() {}
};
class BlockParam {
 public:
	BlockParam();
	~BlockParam();
	int             m_nMapEntryLocalNum;
	int*            m_pMapEntryLocalNumVec;
	int*            m_pMapEntryDisp;
	int             m_nMapEntryGlobalNum;
	MapEntryLocal*   m_pMapTableLocal;
	MapEntryGlobal*  m_pMapTableGlobal;
};

class ImageStitch {
 public:
	ImageStitch();
	~ImageStitch();
	void run();
	void generateStitchParams();
 private:
	static const string FILENAME_PARAMS;
	int            m_nPcnt;
	int            m_nPidx;
	char           m_sPname[MPI_MAX_PROCESSOR_NAME];
	MPI_Datatype   m_mpiDataTypeForMap;
	MPI_Datatype   m_mpiDataTypeForReduce;

	int            m_nScaleCnt;
	int            m_nCameraCnt;
	string         m_sDirLocalImgIn;
	string         m_sFileFormatLocalImgIn;
	string         m_sDirGlobalImgOut;
	string         m_sFileFormatGlobalImgOut;
	string         m_sDirMapTable;
	string         m_sFileFormatMapTable;
	string         m_sDirStitchParam;
	string         m_sFileFormatStitchParam;

	int            m_nHeightGlobal;
	int            m_nWidthGlobal;
	int            m_nHeightBlock;
	int            m_nWidthBlock;
	int            m_nBlockRows;
	int            m_nBlockCols;
	int            m_nBlockCnt;
	int            m_nGtype;
	int            m_nGidx;
	BlockParam*    m_pBlockParams;
	int*           m_pResultLen;

	double         m_dStartTime;
	double         m_dElapseTime;

	Mat            m_imgLocal;
	vector<Pixel*> m_pMapResultVec;
	Pixel*         m_pMapResultGlobal;
	Pixel*         m_pReduceResult;
	Pixel*         m_pReduceResultGlobal;
	Mat            m_imgOk;

	int            m_nFrameIdx;
	bool           m_isOnline;
	int            m_nLeft;
	int            m_nRight;
	int            m_nUp;
	int            m_nDown;
	int            m_onlineScale;
	SOCKET         m_server;
	SOCKET         m_client_exc;

	void  terminate(IN const int errorCode);

	void  tic();
	void  toc();

	void  initMPI();
	void  loadParams();
	void  loadStitchParams();
	void  load(IN FILE* fin);
	void  initInterVar();

	Pixelf biLinInterp(IN const Mat &value, IN const float x, IN const float y);

	bool  loadImgModule(IN const int frameIdx = 0);
	int   mapModule();
	void  gatherModule();
	void  reduceModule();
	void  ImageEachScale(IN const int frameIdx);
};

void pid2gid(IN const int pid ,IN const int cameraCnt,IN const int* blockCnt,OUT int &gid, OUT int &gType);
int gid2pid(IN const int gid, IN const int gType, IN const int cameraCnt ,IN const int blockCnt);

#endif
