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
	std::ifstream infile(ConfigFile);
	infile>>BlockWidth>>BlockHeight;
	infile>>BlockCols>>BlockRows;
	int n=BlockCols*BlockRows+Pcnt;
	int i=0;
	int t=0;
	int *temp=new int [n];
	BlockFetch *p;
	while(i<n)
	{
		infile>>temp[i];
		if(temp[i]<0)
		{
			i=i+1;t=t+1;
		}
		else
		{
			BlocksLists[t]=new BlockFetch;
			BlocksLists[t]->BlockIdx=temp[i];
			BlocksLists[t]->next=NULL;
			p=BlocksLists[t];
			infile>>temp[++i];
			while(temp[i]>=0)
			{
				p=(p->next=new BlockFetch);
				p->BlockIdx=temp[i];
				p->next=NULL;
				infile>>temp[++i];
			}
			i++;t++;
		}
	}
	if(Pidx==0)
	{
		std::cout<<"Pidx 0 Bcast 1 "<<std::endl;
		 for(int h=1;h<Pcnt;h++)		
		{
			BlockFetch *p=BlocksLists[h];
			while(p!=NULL)
			{
				std::cout<<p->BlockIdx<<"   ";
				p=p->next;
				fflush(stdout);
			}
			std::cout<<"\r\n"<<std::endl;
		}
		std::cout<<std::endl;
		fflush(stdout);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&BlockWidth, 1,MPI_INT,Pidx,MPI_COMM_WORLD);
	MPI_Bcast(&BlockHeight,1,MPI_INT,Pidx,MPI_COMM_WORLD);
	MPI_Bcast(&BlockCols,  1,MPI_INT,Pidx,MPI_COMM_WORLD);
	MPI_Bcast(&BlockRows,  1,MPI_INT,Pidx,MPI_COMM_WORLD);
	MPI_Bcast(temp,	   n,MPI_INT,Pidx,MPI_COMM_WORLD);
	ImageBuffer=NULL;
	infile.close();
	delete[]temp;
	return true;
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
std::cout<<"第"<<NumShow<<"次"<<"从客户端收到显示区域数据 ："<<std::endl;
			if(cnt<0)
			{
				std::cout<<"Process 0 think it's not the the require of OFFLINE_WATCH "<<std::endl;
				fflush(stdout);
				break;
			}

			NumShow++;
			for(int i=0;i<8;i++)
			{
				std::cout<<"temp[ "<<i<<" ]"<<" : "<<temp[i]<<" .   ";
				if((i==3)||(i==7))
					std::cout<<std::endl;
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
			while(Rate>1) //Rate难道不是应该为Rate>2
			{
				Rate=Rate>>1;
				Scale++;
			}
			std::cout<<"Scale is  "<<Scale<<std::endl;
			MPI_Bcast(&Frame,1,MPI_INT,Pidx,MPI_COMM_WORLD);
			MPI_Bcast(&Scale,1,MPI_INT,Pidx,MPI_COMM_WORLD);
			MPI_Bcast(&Left, 1,MPI_INT,Pidx,MPI_COMM_WORLD);
			MPI_Bcast(&Right,1,MPI_INT,Pidx,MPI_COMM_WORLD);
			MPI_Bcast(&Up,   1,MPI_INT,Pidx,MPI_COMM_WORLD);
			MPI_Bcast(&Down ,1,MPI_INT,Pidx,MPI_COMM_WORLD);

	//std::cout<<"Left is  "<<Left<<std::endl;
	//std::cout<<"Right is  "<<Right<<std::endl;
	//std::cout<<"Up is  "<<Up<<std::endl;
	//std::cout<<"Down is  "<<Down<<std::endl;

			int get;//?
			int width=(Right-Left)/(1<<Scale);
			int height=(Down-Up)/(1<<Scale);
BufferLength=0;
			if(width*height>BufferLength)  
			{
				BufferLength=width*height;//待初始化；
				if(ImageBuffer!=NULL)
				{
					delete[]ImageBuffer;
				}
				ImageBuffer=new Pixel[BufferLength];
			}
			cv::Mat Image;
			Image=cv::Mat(height,width,CV_8UC1);
			int BlockCnt=BlockCols*BlockRows;
			int *StartX=new int[BlockCnt];
			int *StartY=new int[BlockCnt];
			int *EndX=new int  [BlockCnt];
			int *EndY=new int  [BlockCnt];
			int *BufLen=new int [Pcnt];
			int *BufStart;
			BlockFetch *ProcessList=NULL;//new BlockFetch;
			BlockFetch *pList=ProcessList;
			memset(BufLen,0,sizeof(int)*Pcnt);
			MPI_Request *handle=new MPI_Request[Pcnt];//??1
			MPI_Status status;
			for(int i=1;i<Pcnt;i++)
			{
				BlockFetch *iter=BlocksLists[i];
				bool IsRecv=false;
				while(iter!=NULL)
				{
					int BlockIdx = iter->BlockIdx;	
					int BlockX=iter->BlockIdx%BlockCols;
					int BlockY=iter->BlockIdx/BlockCols;
					int ImageX1=BlockX*BlockWidth;
					int ImageX2=ImageX1+BlockWidth;
					int ImageY1=BlockY*BlockHeight;
					int ImageY2=ImageY1+BlockHeight;
					StartX[BlockIdx]=(max(ImageX1,Left)-Left)/(1<<Scale);
					EndX[BlockIdx]  =(min(ImageX2,Right)-Left)/(1<<Scale);
					StartY[BlockIdx]=(max(ImageY1,  Up)-  Up)/(1<<Scale);
					EndY[BlockIdx]  =(min(ImageY2,Down)-  Up)/(1<<Scale);
					if(EndX[BlockIdx]>StartX[BlockIdx]&&EndY[BlockIdx]>StartY[BlockIdx]&&StartX[BlockIdx]>=0&&StartY[BlockIdx]>=0)
					{							
//cout<<" List   BlockIdx is "<<BlockIdx<<endl; 
						BufLen[i]+=(EndY[BlockIdx]-StartY[BlockIdx])*(EndX[BlockIdx]-StartX[BlockIdx]);
						if(!IsRecv)
						{
							if(pList==NULL)
							{
								pList=new BlockFetch;
								pList->BlockIdx=i;
								pList->next=NULL;
								ProcessList=pList;
							}
							else
							{
								pList=pList->next=new BlockFetch;
								pList->BlockIdx=i;
								pList->next=NULL;
							}
							IsRecv=true;
//cout<<" **List   Pidx is "<<i<<endl; 
						}
					}
					iter=iter->next;
				}
			}
BlockFetch *p=ProcessList;
//if(p==NULL)
//{
//	cout<<"  无进程涉及块  "<<endl;	
//}
//while(p!=NULL)
//{
//	cout<<"  进程  "<<p->BlockIdx<<endl;			
//	p=p->next;
//}
			if(ProcessList==NULL)
			{
cout<<"Over \n"<<endl;
				delete ProcessList;
				memset(ImageBuffer,0,sizeof(Pixel)+BufferLength);
				send(cflag,(char*)ImageBuffer,ScreenWidth*ScreenHeight*sizeof(Pixel),0);
			}
			else
			{

				pList=ProcessList;
				BufStart=new int [Pcnt];
				int i=pList->BlockIdx,j;//i表示进程号

//cout<<"第一个接收进程号  "<<i<<endl;
				BufStart[i]=0;
				MPI_Irecv(ImageBuffer,BufLen[i]*channel,mpiDataForImage,i,0,MPI_COMM_WORLD,&handle[i]);

				while(pList->next!=NULL)
				{
					pList=pList->next;
					j=pList->BlockIdx;			
					BufStart[j]=BufStart[i]+BufLen[i];
					i=j;
cout<<"其余接收进程号  "<<i<<endl;
					MPI_Irecv(ImageBuffer+BufStart[j],BufLen[j]*channel,mpiDataForImage,j,0,MPI_COMM_WORLD,&handle[j]);
				}

				pList->next=ProcessList;	
				memset(Image.data,0,sizeof(Pixel)*Image.cols*Image.rows);
				while(pList!=pList->next)//进程遍历,循环指针的作用是找到最近接收完数据的进程，从而对该进程进行取值。找到该进程后将此从循环链表中去除
				{
					get=false;
					while(!get)
					{
						pList=pList->next;
						i=pList->next->BlockIdx;
						MPI_Test(&handle[i],&get,&status);
					}
					ProcessList=pList->next;
					pList->next=ProcessList->next;
					BlockFetch *iter=BlocksLists[ProcessList->BlockIdx];
					Pixel *pBuffer=ImageBuffer+BufStart[ProcessList->BlockIdx];
std::cout<<"赋值图片的进程号  "<<ProcessList->BlockIdx<<endl;;
					while(iter!=NULL)
					{
						int BlockIdx = iter->BlockIdx;
						if(EndX[BlockIdx]>StartX[BlockIdx]&&EndY[BlockIdx]>StartY[BlockIdx]&&StartX[BlockIdx]>=0&&StartY[BlockIdx]>=0)
						{

cout<<" Booming2   BlockIdx is "<<BlockIdx<<endl; 
							Pixel *pImage=(Pixel*)Image.data+(StartY[BlockIdx]*Image.cols+StartX[BlockIdx]);
							int ColSize=EndX[BlockIdx]-StartX[BlockIdx];
							for(int i=StartY[BlockIdx];i<EndY[BlockIdx];i++)
							{
								memcpy(pImage,pBuffer,ColSize*sizeof(Pixel));
								pBuffer+=ColSize;
								pImage+=Image.cols;
							}
						}
						iter=iter->next;
					}
					delete ProcessList;
				 }

				i=pList->BlockIdx;
std::cout<<"最后一个赋值图片的进程号  "<<pList->BlockIdx<<endl;;
				delete pList;
				get=false;
				while(!get)
				{
					MPI_Test(&handle[i],&get,&status);
				}
				BlockFetch *iter=BlocksLists[i];
				Pixel *pBuffer=ImageBuffer+BufStart[i];
				while(iter!=NULL)//遍历该进程的块，按顺序找出，并通过此对图片进行赋值
				{
					int BlockIdx=iter->BlockIdx;
					//判断在该进程的块中，该块是否包含显示待图片
					if(EndX[BlockIdx]>StartX[BlockIdx]&&EndY[BlockIdx]>StartY[BlockIdx]&&StartX[BlockIdx]>=0&&StartY[BlockIdx]>=0)
					{
						Pixel *pImage=Image.data+StartY[BlockIdx]*Image.cols+StartX[BlockIdx];
						int ColSize=EndX[BlockIdx]-StartX[BlockIdx];
						for(int i=StartY[BlockIdx];i<EndY[BlockIdx];i++)
						{
							memcpy(pImage,pBuffer,ColSize*sizeof(Pixel));
							pImage+=Image.cols;
							pBuffer+=ColSize;
						}
						//对图片进行赋值
					}
					iter=iter->next;
				}


				delete[]handle;
				delete[]BufLen;
				delete[]BufStart;
				delete[]StartX;
				delete[]StartY;
				delete[]EndX;
				delete[]EndY;
				resize(Image,ImageReturn,cv::Size(ScreenWidth,ScreenHeight));
				cv::imwrite("E:/ImageFetch.jpg",Image);
				cv::Mat m_imgGlobalLine=cv::Mat(1,ScreenWidth*ScreenHeight,CV_8UC1);
				ImageReturn.reshape(1,1).copyTo(m_imgGlobalLine);
				send(cflag,(char*)m_imgGlobalLine.data,ScreenWidth*ScreenHeight*sizeof(Pixel),0);

			}
		}
		closesocket(cflag);
		}
		closesocket(sflag);
		WSACleanup();
		return;
}


ImageFetchLocal::ImageFetchLocal(int _Pcnt,int _Pidx,int _root):ImageFetch(_Pcnt,_Pidx) //论_Pcnt,_Pidx的重要性，不加_ gg
{
	root=_root;
	BlocksCnt=0;
	BlocksList=NULL;
}
ImageFetchLocal::~ImageFetchLocal()
{
	BlockFetch *iter=BlocksList;
	while(iter!=NULL)
	{
		iter=iter->next;
		delete BlocksList;
		BlocksList=iter;
	}
}
bool ImageFetchLocal::LoadParameters(const char ConfigFile[])
{
	ImageFormat="E:/Library/data/OfflineImageFetch/ImageOK_98/%d_%d_%d.png";
	int n;
	int i=0;
	int t=0;
	int *temp;
	BlockFetch *p;
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&BlockWidth, 1,MPI_INT,root,MPI_COMM_WORLD);
	MPI_Bcast(&BlockHeight,1,MPI_INT,root,MPI_COMM_WORLD);
	MPI_Bcast(&BlockCols  ,1,MPI_INT,root,MPI_COMM_WORLD);
	MPI_Bcast(&BlockRows  ,1,MPI_INT,root,MPI_COMM_WORLD);
	n=BlockCols*BlockRows+Pcnt;
	temp=new int [n];
	MPI_Bcast(temp,n,MPI_INT,root,MPI_COMM_WORLD);
	while(i<n&&t<=Pidx)//Pidx represents the process id
	{
		if(temp[i]<0)
		{
			i++;t++;
		}
		else if(t==Pidx)
		{
			p=BlocksList=new BlockFetch;
			BlocksList->BlockIdx=temp[i++];
			BlocksList->next=NULL;
			BlocksCnt=1;
			while(temp[i]>=0)
			{
				p=(p->next=new BlockFetch);
				p->BlockIdx=temp[i++];
				p->next=NULL;
				BlocksCnt++;
			}
			break;
		}
		else
		{
			while(temp[i++]>=0)
			{
				;
			}
			t++;
		}
	}
	p=BlocksList;
	cout<<"Pidx is  "<<setw(6)<<Pidx<<"  Blocck list is ";
	while(p!=NULL)
	{
		cout<<setw(4)<<p->BlockIdx<<" ";
		p=p->next;
	}
	std::cout<<endl;
	fflush(stdout);
	delete[]temp;
	BufferLength=BlockWidth*BlockHeight*BlocksCnt;
	if(ImageBuffer!=NULL)
	{
		delete[]ImageBuffer;	
	}
	ImageBuffer=new Pixel[BufferLength];//待显示，待发送图片的指针
	return 1;
}

void ImageFetchLocal::Run()
{
	std::string ConfigFile="";
	LoadParameters(ConfigFile.data());
	int cnt=0;
	bool isRun=false;
	while(true)
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
		{
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt,1,MPI_INT,root,MPI_COMM_WORLD);
			if(cnt<=0)
			{
				break;
			}
			MPI_Bcast(&Frame,1,MPI_INT,root,MPI_COMM_WORLD);
			MPI_Bcast(&Scale,1,MPI_INT,root,MPI_COMM_WORLD);
			MPI_Bcast(&Left ,1,MPI_INT,root,MPI_COMM_WORLD);
			MPI_Bcast(&Right,1,MPI_INT,root,MPI_COMM_WORLD);
			MPI_Bcast(&Up,   1,MPI_INT,root,MPI_COMM_WORLD);
			MPI_Bcast(&Down, 1,MPI_INT,root,MPI_COMM_WORLD);
//std::cout<<"Pidx "<<Pidx<<" has cast the six parameters ."<<std::endl;
			BlockFetch *iter=BlocksList;
			Pixel *pBuffer=ImageBuffer;
			while(iter!=NULL)
			{
				int BlockX=iter->BlockIdx%BlockCols;
				int BlockY=iter->BlockIdx/BlockCols;
				int ImageX1=BlockX*BlockWidth;
				int ImageX2=ImageX1+BlockWidth;
				int ImageY1=BlockY*BlockHeight;
				int ImageY2=ImageY1+BlockHeight;
				int StartX=(max(ImageX1,Left)-Left)/(1<<Scale);
				int EndX  =(min(ImageX2,Right)-Left)/(1<<Scale);
				int StartY=(max(ImageY1,  Up)-  Up)/(1<<Scale);
				int EndY  =(min(ImageY2,Down)-  Up)/(1<<Scale);


				if(EndX>StartX&&EndY>StartY&&StartX>=0&&StartY>=0) //待显示图片在该进程对应的某个块中时
				{	
//std::cout<<"Bong local  "<<"BlockIdx  "<<setw(5)<<iter->BlockIdx<<"   Pidx  "<<Pidx<<endl;
					sprintf(ImageDirectory,ImageFormat.data(),Scale,iter->BlockIdx,Frame);
					cv::Mat Image=cv::imread(ImageDirectory,CV_LOAD_IMAGE_GRAYSCALE);

					if(!Image.isContinuous())
					{
						std::cout<<"Local Image is not continuous !"<<std::endl;
					}	
					int StartXlocal=(max(ImageX1,Left)-ImageX1)/(1<<Scale);
					int EndXlocal  =(min(ImageX2,Right)-ImageX1)/(1<<Scale);
					int StartYlocal=(max(ImageY1,Up)  -ImageY1)/(1<<Scale);
					int EndYlocal  =(min(ImageY2,Down)-ImageY1)/(1<<Scale);
				

//Mat roi_img = Image(Range(StartXlocal,EndXlocal),Range(StartYlocal,EndYlocal));
Mat roi_img = Image(Range(StartYlocal,EndYlocal),Range(StartXlocal,EndXlocal));//?优化的地方
char aaaaa[255];
sprintf(aaaaa, "E:\\store\\BadPidx_%d_read_number_%d.png",Pidx,iter->BlockIdx);
imwrite(aaaaa,roi_img);				 
std::cout<<" StartXlocal  "<<StartXlocal<<" . EndXlocal  "<<EndXlocal<<" . StartYlocal "<<StartYlocal<<" .  EndYlocal  "<<EndYlocal<<endl;

					Pixel *pImage=(Pixel*)Image.data+(StartYlocal*BlockWidth)/(1<<Scale)+StartXlocal;
std::cout<<" Start address  "<<(StartYlocal*BlockWidth)/(1<<Scale)+StartXlocal<<endl;
					int ColSize=EndX-StartX;
					for(int i=StartY;i<EndY;i++)
					{
						memcpy(pBuffer,pImage,ColSize*sizeof(Pixel));
						pBuffer+=ColSize;
						pImage+=Image.cols;
					}
				}
				iter=iter->next;
			}
				int BufLen=pBuffer-ImageBuffer;

//std::cout<<"Pidx is "<<Pidx<<" Weather send data to Comm. DataLen is "<<BufLen<<std::endl;
//fflush(stdout);
				if(BufLen>0)
				{
std::cout<<"Pidx is "<<Pidx<<" Should send data to Comm. DataLen is "<<BufLen<<std::endl;
fflush(stdout);
					MPI_Send(ImageBuffer,BufLen*channel,mpiDataForImage,root,0,MPI_COMM_WORLD);
std::cout<<"Pidx is "<<Pidx<<" Have   send data to Comm. DataLen is "<<BufLen<<std::endl;
fflush(stdout);

				}
			

		}
	}
}


