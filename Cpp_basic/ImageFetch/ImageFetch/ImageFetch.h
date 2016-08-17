#ifndef IMAGE_FETCH_H
#define IMAGE_FETCH_H
#include"stdafx.h"
//#include"Pixel.h"

 

typedef struct Block_fetch
{
	int BlockIdx;
	struct Block_fetch *next;
}BlockFetch;


class ImageFetch
{
public:
 	ImageFetch(int _Pcnt,int _Pidx);
 	virtual ~ImageFetch()=0;
	virtual void Run()=0;
protected:
	int Pidx,Pcnt;
	MPI_Datatype mpiDataForImage;
	int channel;
	int BlockRows,BlockCols;
	int BlockWidth,BlockHeight;
	int Frame,Scale;
	int Left,Right,Up,Down;
	int BufferLength;
 	Pixel *ImageBuffer;
};
class ImageFetchGlobal:public ImageFetch
{
public:
	ImageFetchGlobal(int _Pcnt,int _Pidx);
 	~ImageFetchGlobal();
	void Run();
private:
	bool LoadParameters(const char ConfigFile[]);
	void tic();
	double toc();
	BlockFetch **BlocksLists;
	int ScreenWidth,ScreenHeight;
	double StartTime,ElapseTime;
	cv::Mat ImageReturn;
};


#endif