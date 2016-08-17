#include"ImageFetch.h"
#include"stdafx.h"
#define SOCKET_BUFFER_LENGTH 32  //修改前256
#define SOCKET_COM 6666

ImageFetch::ImageFetch(int _Pcnt,int _Pidx)
{
	Pcnt=_Pcnt;
	Pidx=_Pidx;
	if(sizeof(Pixel)==1||sizeof(Pixel)==3)
	{
		mpiDataForImage=MPI_CHAR;
	}
	channel=1;
	BlockRows=0;
	BlockCols=0;
	BlockWidth=0;
	BlockHeight=0;
	Frame=0;
	Scale=0;
	Left=0;
	Right=0;
	Up=0;
	Down=0;
	ImageBuffer=NULL;
	BufferLength=0;
}
ImageFetch::~ImageFetch(){}
ImageFetchGlobal::ImageFetchGlobal(int _Pcnt,int _Pidx):ImageFetch(_Pcnt, _Pidx)
{
	ScreenWidth=0;
	ScreenHeight=0;
	StartTime=0;
	ElapseTime=0;
	BlocksLists=new BlockFetch*[Pcnt];
	for(int i=0;i<_Pcnt;i++)
	{
		BlocksLists[i]=NULL;
	}
}
ImageFetchGlobal::~ImageFetchGlobal()
{
	BlockFetch *iter;
	if(BlocksLists!=NULL)
	{
		for(int i=0;i<Pcnt;i++)
		{
			iter=BlocksLists[i];
			while(iter!=NULL)
			{
				iter=iter->next;
				delete BlocksLists[i];
				BlocksLists[i]=iter;
			}
		}
		delete[]BlocksLists;
	}
	if(ImageBuffer!=NULL)
	{
		delete []ImageBuffer;
	}
}
void ImageFetchGlobal::tic()
{
	StartTime=MPI_Wtime();
}
double ImageFetchGlobal::toc()
{
	ElapseTime=MPI_Wtime()-StartTime;
	return ElapseTime;
}
bool ImageFetchGlobal::LoadParameters(const char ConfigFile[])
{
	std::cout<<"未载入参数"<<std::endl;
	fflush(stdout);
	return 0;
}
void ImageFetchGlobal::Run()
{
	std::string ConfigFile="E:/Library/data/OfflineImageFetch/process/Process.txt";
		fflush(stdout);
	LoadParameters(ConfigFile.data());

	int cnt=0;
	bool isRun=false;
	char recvBuf[SOCKET_BUFFER_LENGTH];
	SOCKET sflag,cflag;
	sockaddr_in sin,remote;
	WORD sockVersion=MAKEWORD(2,2);
	WSADATA wsaData;
	if(WSAStartup(sockVersion,&wsaData)!=0)
	{
		std::cout<<"Process could not start up WSAStartup"<<std::endl;
		fflush(stdout);
		return;
	}
	sflag=socket(AF_INET,SOCK_STREAM,0);
	if(sflag<0)
	{
		std::cout<<"Process could not Socket"<<std::endl;
		fflush(stdout);
		return;
	}
	sin.sin_family=AF_INET;
	sin.sin_port=htons(SOCKET_COM);
	sin.sin_addr.s_addr=INADDR_ANY;
	if(bind(sflag,(LPSOCKADDR)&sin,sizeof(sin))==SOCKET_ERROR)
	{
		std::cout<<"Process could not bind"<<std::endl;
		fflush(stdout);
		return;
	}
	if(listen(sflag,5)==SOCKET_ERROR)
	{
		std::cout<<"Process could not listen"<<std::endl;
		fflush(stdout);
		return ;
	}
	socklen_t addr_len=sizeof(remote);
	while(true)
	{
		std::cout<<"Process "<<Pidx<<" waiting for connection "<<std::endl;
		fflush(stdout);
		cflag=accept(sflag,(LPSOCKADDR)&remote,(socklen_t*)&addr_len);
		if(cflag==INVALID_SOCKET)
		{
			isRun=false;
		}
		else
		{
			isRun=true;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&isRun,sizeof(isRun),MPI_CHAR,Pidx,MPI_COMM_WORLD);
		if(!isRun)
		{
			std::cout<<"Process 0 Bcast judge isRun failed "<<std::endl;
			fflush(stdout);
			continue;
		}
	int NumShow=1;
		while(true)
		{
			cnt=recv(cflag,recvBuf,SOCKET_BUFFER_LENGTH,0);
			int *temp=(int*)recvBuf;
			if((cnt>0&&temp[0]!=OFFLINE_WATCH)||cnt<8*sizeof(int))
			{
					cnt=-1;
			}
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt,1,MPI_INT,Pidx,MPI_COMM_WORLD);
			if(cnt<0)
			{
				std::cout<<"Process 0 think it's not the the require of OFFLINE_WATCH "<<std::endl;
				fflush(stdout);
				break;
			}
			std::cout<<"第"<<NumShow<<"次"<<"从客户端收到显示区域数据 ："<<std::endl;
			NumShow++;
			for(int i=0;i<8;i++)
			{
				std::cout<<"temp[ "<<i<<" ]"<<" : "<<temp[i]<<" .   ";
			}
			Frame=temp[1];
			ScreenWidth=temp[2];
			ScreenHeight=temp[3];
			Left=temp[4];
			Right=temp[5];
			Up=temp[6];
			Down=temp[7];
			if(Left<0)
				Left=0;
			if(Right>12720)
				Right=12720;
			if(Up<0)
				Up=0;
			if(Down>15000)
				Down=15000;
			if(Left>=Right)
				Left=Right-1000;
			if(Up>=Down)
				Up=Down-1000;
			Scale=0;
			int Rate=min((Right-Left)/ScreenWidth,(Down-Up)/ScreenHeight);
			std::cout<<" ";///////////////////**********************/////////////////
		}
	}

	/*while(true)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&isRun,1,MPI_CHAR,root,MPI_COMM_WORLD);
		if(!isRun)
		{
			continue;
		}
		char ImageDirectory[255];
		cv::Mat LocalImage;
		while(true)
}*/

}