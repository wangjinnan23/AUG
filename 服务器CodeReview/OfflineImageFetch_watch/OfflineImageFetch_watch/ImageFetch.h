#ifndef IMAGE_FETCH_H
#define IMAGE_FETCH_H

#include "stdafx.h"
#include "Pixel.h"

typedef struct Block_Fetch 
{
	int BlockIdx;
	struct Block_Fetch *next;
} BlockFetch;

class ImageFetch 
{
 public:
	ImageFetch(int _Pcnt, int _Pidx);
	virtual ~ImageFetch() = 0;
	virtual void Run() = 0;
 protected:
	int Pcnt, Pidx; // Process number, Process id
	MPI_Datatype mpiDataForImage;
	int channel;
	int BlockRows, BlockCols;
	int BlockWidth, BlockHeight; 
	int Frame, Scale;
	int Left, Right, Up, Down;
	int BufferLength;
	Pixel *ImageBuffer;
};

class ImageFetchLocal:public ImageFetch 
{
 public:
	ImageFetchLocal(int _Pcnt, int _Pidx, int _root);
	~ImageFetchLocal();
	void Run();
 private:
	bool LoadParameters(const char ConfigFile[]);
	
	int root;
	BlockFetch *BlocksList;
	int BlocksCnt;
	string ImageFormat;
	
};

class ImageFetchGlobal:public ImageFetch 
{
 public:
	ImageFetchGlobal(int _Pcnt, int _Pidx);
	~ImageFetchGlobal();
	void Run();
 private:
	bool LoadParameters(const char ConfigFile[]);
	void tic();
	double toc();

	BlockFetch **BlocksLists;
	int ScreenWidth, ScreenHeight;
	double StartTime, ElapseTime;
	cv::Mat ImageReturn;
	
};

#endif
