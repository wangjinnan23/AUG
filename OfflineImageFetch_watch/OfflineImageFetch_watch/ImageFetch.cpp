#include "ImageFetch.h"
#include "Message.h"

#define min(x,y) ((x)<(y) ? (x) : (y))
#define max(x,y) ((x)>(y) ? (x) : (y))
#define SOCKET_COM 6666
#define SOCKET_BUFFER_LENGTH 256
ImageFetch::ImageFetch(int _Pcnt, int _Pidx) {
	Pcnt = _Pcnt;
	Pidx = _Pidx;
	if(sizeof(Pixel) == 1 || sizeof(Pixel) == 3) mpiDataForImage = MPI_CHAR;
	else if(sizeof(Pixel) == 2 || sizeof(Pixel) == 6) mpiDataForImage = MPI_SHORT;
	else mpiDataForImage = MPI_DATATYPE_NULL;
	if(sizeof(Pixel) % 3 == 0) channel = 3;
	else channel = 1;
	BlockRows = 0;
	BlockCols = 0;
	BlockWidth = 0;
	BlockHeight = 0;
	Frame = 0;
	Scale = 0;
	Left = 0;
	Right = 0;
	Up = 0;
	Down = 0;
	ImageBuffer = NULL;
	BufferLength = 0;
}
ImageFetch::~ImageFetch() {}
ImageFetchLocal::ImageFetchLocal(int _Pcnt, int _Pidx, int _root):ImageFetch(_Pcnt, _Pidx) {
	root = _root;
	BlocksCnt = 0;
	BlocksList = NULL;
}
ImageFetchLocal::~ImageFetchLocal() {
	BlockFetch *iter = BlocksList;
	while(iter != NULL) {
		iter = iter->next;
		delete BlocksList;
		BlocksList = iter;
	}
	if(ImageBuffer != NULL) delete[]ImageBuffer;
}
bool ImageFetchLocal::LoadParameters(const char ConfigFile[]) {
//	ImageFormat = "/home/westly/Desktop/Workspace/ImageStitch/TestData/ovary/ImageOK_98/%d_%d_%d.png";


	ImageFormat = "E:\\Library\\data\\OfflineImageFetch\\ImageOK_98\\%d_%d_%d.png";


	int n, i = 0, t = 0, *temp;
	BlockFetch *p;
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&BlockWidth, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&BlockHeight, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&BlockCols, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&BlockRows, 1, MPI_INT, root, MPI_COMM_WORLD);
	n = BlockCols * BlockRows + Pcnt;
	temp = new int[n];
	MPI_Bcast(temp, n, MPI_INT, root, MPI_COMM_WORLD);
	while(i < n && t <= Pidx)   //t代表进程序号
		if(temp[i] < 0) 
		{
			i = i + 1; t = t + 1;
		} 
		else if(t == Pidx) 
		{
			p = BlocksList = new BlockFetch;
			BlocksList->BlockIdx = temp[i++];
//	cout<<"blockIdx = "<<temp[i-1]<<"  Pidx = "<<Pidx<<endl;
			BlocksList->next = NULL;
			BlocksCnt = 1;
			while(temp[i] >= 0) 
			{
				p = (p->next = new BlockFetch);
				p->BlockIdx = temp[i++];

//cout<<"blockIdx = "<<temp[i-1]<<"  Pidx = "<<Pidx<<endl;

				p->next = NULL;
				BlocksCnt++;
			}
			break;
		
		} 
		else 
		{
			while(temp[i++] >= 0);
			t = t + 1;
		}
	delete[]temp;
	BufferLength = BlockWidth * BlockHeight * BlocksCnt;    
	if(ImageBuffer != NULL) delete[]ImageBuffer;
	ImageBuffer = new Pixel[BufferLength];
	return true;
}
void ImageFetchLocal::Run() {
	string ConfigFile = "";
	LoadParameters(ConfigFile.data());    //传递该进程所在的块链接指针BlocksList

	cout<<"Pidx:  "<<Pidx<<" after LoadParameters(ConfigFile.data())"<<endl;
	int cnt = 0;
	bool isRun = false;
	
//	bool isRun = true;

	while(true) 
	{
       MPI_Barrier(MPI_COMM_WORLD);
  	   MPI_Bcast(&isRun, 1, MPI_CHAR, root, MPI_COMM_WORLD);
		if(!isRun) continue;
		char ImageDirectory[255];
		Mat LocalImage;

		while(true) 
		{
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt, 1, MPI_INT, root, MPI_COMM_WORLD);
			if(cnt <= 0) break;
			MPI_Bcast(&Frame, 1, MPI_INT, root, MPI_COMM_WORLD);
			MPI_Bcast(&Scale, 1, MPI_INT, root, MPI_COMM_WORLD);
			MPI_Bcast(&Left, 1, MPI_INT, root, MPI_COMM_WORLD);
			MPI_Bcast(&Right, 1, MPI_INT, root, MPI_COMM_WORLD);
			MPI_Bcast(&Up, 1, MPI_INT, root, MPI_COMM_WORLD);
			MPI_Bcast(&Down, 1, MPI_INT, root, MPI_COMM_WORLD);
			BlockFetch *iter = BlocksList;
			Pixel *pBuffer = ImageBuffer;

			while(iter != NULL)
			{
				int BlockX = iter->BlockIdx % BlockCols,
				    BlockY = iter->BlockIdx / BlockCols;
				int ImageX1 = BlockX * BlockWidth,
				    ImageX2 = ImageX1 + BlockWidth,
				    ImageY1 = BlockY * BlockHeight,
				    ImageY2 = ImageY1 + BlockHeight;
				//int StartX = (max(ImageX1, Left) - ImageX1) / (1 << Scale),
				//    EndX = (min(ImageX2, Right) - ImageX1) / (1 << Scale),
				//    StartY = (max(ImageY1, Up) - ImageY1) / (1 << Scale),
				//    EndY = (min(ImageY2, Down) - ImageY1) / (1 << Scale);


				int StartX = (max(ImageX1, Left) - Left) / (1 << Scale),
				    EndX = (min(ImageX2, Right) - Left) / (1 << Scale),
				    StartY =  (max(ImageY1, Up) - Up) / (1 << Scale),
				    EndY = (min(ImageY2, Down) - Up) / (1 << Scale);

if(Pidx==5||Pidx==7)
std::cout<<"Pidx  "<<Pidx<<"   BlockIdx  "<<setw(5)<<iter->BlockIdx<<endl;
				if(EndX > StartX && EndY > StartY && StartX >= 0 && StartY >= 0) 
				{
					sprintf(ImageDirectory, ImageFormat.data(), Scale, iter->BlockIdx, Frame);
std::cout<<"Bong  "<<"BlockIdx  "<<setw(5)<<iter->BlockIdx<<"   Pidx  "<<Pidx<<endl;
					Mat Image = imread(ImageDirectory,CV_LOAD_IMAGE_GRAYSCALE);
/*std::cout<<"Pidx "<<Pidx<<" success read localimage in: "<<ImageDirectory<<std::endl;
std::cout<<"BlockIdx "<<setw(5)<<iter->BlockIdx<<setw(7)<<" ImageX1 "<<setw(5)<< ImageX1 <<" ImageX2 "<<setw(5)<<ImageX2 <<" ImageY1 "<<setw(5)<<ImageY1<<" ImageY2 "<<setw(5)<<ImageY2 <<std::endl;
std::cout<<"The location  "<<": StartX "<<setw(5)<<StartX<<"  EndX "<<setw(5)<<EndX<<"  StartY "<<setw(5)<<StartY<<"  EndY "<<setw(5)<<EndY<<std::endl;
fflush(stdout);		*/				


					 

					if(channel == 3) cvtColor(Image, LocalImage, CV_RGB2GRAY);
				    else LocalImage = Image;
                    

					
					if(!Image.isContinuous())
					{
						std::cout<<"Local Image is not continuous"<<endl;			
					}

					int StartYlocal = (max(ImageY1, Up) - ImageY1) / (1 << Scale);
					int EndYlocal = (min(ImageY2, Down) - ImageY1) / (1 << Scale);
					int StartXlocal = (max(ImageX1, Left) - ImageX1) / (1 << Scale);				
					int EndXlocal = (min(ImageX2, Right) - ImageX1) / (1 << Scale);

Mat roi_img = Image(Range(StartYlocal,EndYlocal),Range(StartXlocal,EndXlocal));
char aaaaa[255];
sprintf(aaaaa, "E:\\store\\GoodPidx_%d_read_number_%d.png",Pidx,iter->BlockIdx);
imwrite(aaaaa,roi_img);
					Pixel *pImage = (Pixel*)Image.data + (StartYlocal * BlockWidth )/( 1<<Scale)+StartXlocal ;

				//	Pixel *pImage = (Pixel*)Image.data + StartY * BlockWidth  /( 1<<Scale)+ StartX;

			//	cout<<"  StartX  is "<<StartX<<" ;  EndX  is "<<EndX<<" ;  StartY  is "<<StartY<<" ;  EndY  is "<<EndY<<"  BlockWidth ="<<BlockWidth <<" scale "<< (1<<Scale)<<endl;
			//	cout<<"  StartXlocal  is "<<StartXlocal<<" ;  EndXlocal  is "<<EndXlocal<<" ;  StartYlocal  is "<<StartYlocal<<" ;  EndYlocal  is "<<EndYlocal<<"  BlockWidth ="<<BlockWidth <<" scale "<< (1<<Scale)<<endl;
					int ColSize = EndX - StartX;

					cv::Mat tmp1(1024,768,CV_8UC3,cv::Scalar(0,0,0));
					cv::Mat tmp2(1024,768,CV_8UC3,cv::Scalar(0,0,0));

			//		std::cout<<"pImage: "<<(int)pImage<<" Image start address: "<<(int)Image.data<<"  end address: "<<(int)(Image.ptr(Image.rows-1)+Image.cols)<<endl;

					for(int i = StartY; i < EndY; i++) 
					{
					//	std::cout<<"pBuffer: "<<(int)pBuffer<<"  pImage: "<<(int)pImage<<"  size:"<<ColSize * sizeof(Pixel)<<endl;
						//for(int j=0;j<ColSize*sizeof(Pixel);++j){
						//	std::cout<<*(tmp1.data+j)<<" ";
						//	//*(pBuffer+j)=*(pImage+j);
						//}
						memcpy(pBuffer, pImage, ColSize * sizeof(Pixel));
					
						
						//memcpy(pBuffer,tmp1.data,1000);
						pBuffer += ColSize;
						   pImage += Image.cols;
					
					}

				/*cout<<"Pidx: "<<Pidx<<"  iter-blockidx = "<<iter->BlockIdx<<"该块取多少点 The number of the sampled block is  "<<int( pBuffer-ImageBuffer) <<endl;
				fflush(stdout);*/

				}
				iter = iter->next;
			}

			int BufLen = pBuffer - ImageBuffer;
		//	cout<<" Process Pidx:  "<<Pidx<<" will send number is Buflen  = "<<BufLen<<endl; 

			if(BufLen > 0)
				{ 
std::cout<<"Pidx is "<<Pidx<<" Should send data to Comm. DataLen is "<<BufLen<<std::endl;
fflush(stdout);
			MPI_Send(ImageBuffer, BufLen * channel, mpiDataForImage, root, 0, MPI_COMM_WORLD);
std::cout<<"Pidx is "<<Pidx<<" Have   send data to Comm. DataLen is "<<BufLen<<std::endl;
fflush(stdout);
			}
		}
	}
}

ImageFetchGlobal::ImageFetchGlobal(int _Pcnt, int _Pidx):ImageFetch(_Pcnt, _Pidx) 
{
	ScreenWidth = 0;
	ScreenHeight = 0;
	StartTime = 0;
	ElapseTime = 0;
	BlocksLists = new BlockFetch*[_Pcnt];
	for(int i = 0; i < _Pcnt; i++) 
	{
		BlocksLists[i] = NULL;
	}
}
ImageFetchGlobal::~ImageFetchGlobal()
{
	BlockFetch *iter;
	if(BlocksLists != NULL) 
	{
		for(int i = 0; i < Pcnt; i++)
		{
			iter = BlocksLists[i];
			while(iter != NULL)
			{
				iter = iter->next;
				delete BlocksLists[i];
				BlocksLists[i] = iter;
			}
		}
		delete[]BlocksLists;
	}
	if(ImageBuffer != NULL) delete[]ImageBuffer;
}
void ImageFetchGlobal::tic() 
{
	StartTime = MPI_Wtime();
}
double ImageFetchGlobal::toc()
{
	ElapseTime = MPI_Wtime() - StartTime;
	return ElapseTime;
}
bool ImageFetchGlobal::LoadParameters(const char ConfigFile[]) 
{
	ifstream infile(ConfigFile);
	infile>>BlockWidth>>BlockHeight;
	infile>>BlockCols>>BlockRows;
	 
cout<<"BlockWidth:"<<BlockWidth<<"  BlockHeight:"<<BlockHeight<<"  BlockCols"<<BlockCols<<"  BlockRows"<<BlockRows<<endl;


	int n = BlockCols * BlockRows + Pcnt, i = 0, t = 0;
	int *temp = new int[n];
	BlockFetch *p;


	while(i < n)
	{
		infile >> temp[i]; 

//cout<<"duru temp[i] = "<<temp[i]<<"  i = "<<i<<endl;

		if(temp[i] < 0) 
		{
			i = i + 1; t = t + 1;
		} 
		else 
		{
			BlocksLists[t] = new BlockFetch;
			BlocksLists[t]->BlockIdx = temp[i];



			BlocksLists[t]->next = NULL;
			p = BlocksLists[t];
			infile >> temp[++i];
//cout<<"duru temp[i] = "<<temp[i]<<"  i = "<<i<<endl;

			while(temp[i] >= 0)
			{
				p = (p->next = new BlockFetch);
				p->BlockIdx = temp[i];
				p->next = NULL;
				infile >> temp[++i];

//cout<<"duru temp[i] = "<<temp[i]<<"  i = "<<i<<endl;

			}
			i = i + 1; t = t + 1;
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
cout<<"barrier 之后"<<endl;
	MPI_Bcast(&BlockWidth, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
	MPI_Bcast(&BlockHeight, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
	MPI_Bcast(&BlockCols, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
	MPI_Bcast(&BlockRows, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
	MPI_Bcast(temp, n, MPI_INT, Pidx, MPI_COMM_WORLD);

	ImageBuffer = NULL;
	infile.close();
delete []temp;

	return true;
}
void ImageFetchGlobal::Run() 
{
///	string ConfigFile = "/home/westly/Desktop/Workspace/ImageStitch/TestData/Process.txt";

	string ConfigFile = "E:\\Library\\data\\OfflineImageFetch\\process\\Process.txt";

	LoadParameters(ConfigFile.data());
cout<<"load 之后"<<endl;
	int cnt = 0;
	bool isRun = false;
	char recvBuf[SOCKET_BUFFER_LENGTH];
	SOCKET sflag, cflag;
	sockaddr_in sin, remote;
//#ifdef _WINDOWS
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if(WSAStartup(sockVersion, &wsaData) != 0)
	{
		cout << "Process could not start up WSA!\n";
		return;
	}
//#endif
	sflag = socket(AF_INET, SOCK_STREAM, 0);
	if(sflag < 0) return;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SOCKET_COM);
	sin.sin_addr.s_addr = INADDR_ANY;
	if(bind(sflag, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) return;
	if(listen(sflag, 5) == SOCKET_ERROR) return;
	socklen_t  addr_len = sizeof(remote);



	while(true)
	{
		cout<<"Pidx "<<Pidx<<" Waiting for connecting!\n"<<endl;

		cflag = accept(sflag, (LPSOCKADDR)&remote, (socklen_t*)&addr_len);


		if(cflag == INVALID_SOCKET) isRun = false;
		else isRun = true;
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&isRun, sizeof(isRun), MPI_CHAR, Pidx, MPI_COMM_WORLD);
		if(!isRun) continue;

int jieshoucishu = 1;

		while(true) 
		{
			cnt = recv(cflag, recvBuf, SOCKET_BUFFER_LENGTH, 0);
			int *temp = (int*)recvBuf;
			if((cnt > 0 && temp[0] != FETCH) || cnt < 8 * sizeof(int)) cnt = -1;
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Bcast(&cnt, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			if(cnt <= 0) break;
			for(int i = 0; i < 8; i++) cout<<temp[i]<<" "; cout<<endl;

			cout<<"jieshoucanshucishu:   "<<jieshoucishu<<endl;

			Frame = temp[1];
			ScreenWidth = temp[2];
			ScreenHeight = temp[3];
			Left = temp[4];
			Right = temp[5];
			Up = temp[6];
			Down = temp[7];
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
			Scale = 0;
			int Rate = min((Right - Left) / ScreenWidth, (Down - Up) / ScreenHeight);
			while(Rate > 1)
			{
				Rate = Rate >> 1;
				Scale++;
			}

			cout<<"  !!!!  compute scale from the client data is "<<Scale<<endl;

			MPI_Bcast(&Frame, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			MPI_Bcast(&Scale, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			MPI_Bcast(&Left, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			MPI_Bcast(&Right, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			MPI_Bcast(&Up, 1, MPI_INT, Pidx, MPI_COMM_WORLD);
			MPI_Bcast(&Down, 1, MPI_INT, Pidx, MPI_COMM_WORLD);

			int get;
			int width = (Right - Left) / (1 << Scale),
			    height = (Down - Up) / (1 << Scale);
			if(width * height > BufferLength) 
			{
				BufferLength = width * height;
				if(ImageBuffer != NULL) delete[]ImageBuffer;
				ImageBuffer = new Pixel[BufferLength];
			}
			Mat Image;
			switch(sizeof(Pixel))
			{
			case 1: Image = Mat(height, width, CV_8UC1); break;
			case 3: Image = Mat(height, width, CV_8UC3); break;
			case 2: Image = Mat(height, width, CV_16UC1); break;
			case 6: Image = Mat(height, width, CV_16UC3); break;
			default:break;
			}
			int BlockCnt = BlockCols * BlockRows;
			int *StartX = new int[BlockCnt],
			    *StartY = new int[BlockCnt],
			    *EndX = new int[BlockCnt],
			    *EndY = new int[BlockCnt];
			int *BufLen = new int[Pcnt], *BufStart;
			BlockFetch *ProcessList = new BlockFetch, *pList = ProcessList;
//			cout<<"Pro addr "<<ProcessList<<"   plist addr "<<pList<<endl;
			memset(BufLen, 0, sizeof(int) * Pcnt);
			MPI_Request *handle = new MPI_Request[Pcnt];
			MPI_Status status;



			for(int i = 0; i < Pcnt; i++)
				if(i != Pidx)
				{
					BlockFetch *iter = BlocksLists[i];
					bool IsRecv = false;
					while(iter != NULL)
					{
						int BlockIdx = iter->BlockIdx,
						    BlockX = BlockIdx % BlockCols,
						    BlockY = BlockIdx / BlockCols;
						int ImageX1 = BlockX * BlockWidth,
						    ImageX2 = ImageX1 + BlockWidth,
						    ImageY1 = BlockY * BlockHeight,
						    ImageY2 = ImageY1 + BlockHeight;
						StartX[BlockIdx] = (max(ImageX1, Left) - Left) / (1 << Scale);
						EndX[BlockIdx] = (min(ImageX2, Right) - Left) / (1 << Scale);
						StartY[BlockIdx] = (max(ImageY1, Up) - Up) / (1 << Scale);
						EndY[BlockIdx] = (min(ImageY2, Down) - Up) / (1 << Scale);
						if(EndX[BlockIdx] > StartX[BlockIdx] &&
						   EndY[BlockIdx] > StartY[BlockIdx] &&
						   StartX[BlockIdx] >= 0 && StartY[BlockIdx] >= 0)
						{
							BufLen[i] += (EndY[BlockIdx] - StartY[BlockIdx]) * 
							             (EndX[BlockIdx] - StartX[BlockIdx]);
                         //     cout<<"第 "<<BlockIdx<<" 块中需要的点数："<< (EndY[BlockIdx] - StartY[BlockIdx]) * (EndX[BlockIdx] - StartX[BlockIdx])<<endl;;
                          //  cout<<"有块 cunzai"<<endl;
							if(!IsRecv)
							{
								pList = pList->next = new BlockFetch;
								IsRecv = true;
								pList->BlockIdx = i; //此时i表示进程号，存储的是进程号
							}
						}
						iter = iter->next;
					}
				}

			//	cout<<"jincheng id1 = "<<ProcessList->BlockIdx<<"  id2="<<ProcessList->next->BlockIdx<<"  id3="<<ProcessList->next->next->BlockIdx<<endl;

			if(pList == ProcessList) 
			{
				delete ProcessList;
				memset(ImageBuffer, 0, sizeof(Pixel) * BufferLength);
				send(cflag, (char *)ImageBuffer, ScreenWidth * ScreenHeight * sizeof(Pixel), 0);

				cout<<"ImageBuffer[3]:"<<ImageBuffer[3]<<"    ImageBuffer[30]:"<<ImageBuffer[30]<<endl;
				cout<<"send image over1"<<endl;

			} 
			else 
			{
				pList->next = ProcessList->next;
		
				delete ProcessList;
				ProcessList = pList;
				BufStart = new int[Pcnt];
				int i = pList->BlockIdx, j;  //i表示的进程号
				BufStart[i] = 0;
				MPI_Irecv(ImageBuffer, BufLen[i] * channel, mpiDataForImage, i, 0, MPI_COMM_WORLD, &handle[i]); 
			

				//cout<<"each process send points number :BufLen[i] "<<BufLen[i]<<", fasongjinchenghao i = "<<i<<" Pidx "<<Pidx<<endl;

				pList = pList->next;
				while(pList != ProcessList)
				{
					j = pList->BlockIdx;
					BufStart[j] = BufStart[i] + BufLen[i];
					i = j;
				MPI_Irecv(ImageBuffer + BufStart[j], BufLen[j] * channel, mpiDataForImage, j, 0, MPI_COMM_WORLD, &handle[j]);
                
					pList = pList->next;				
				}

				memset(Image.data, 0, sizeof(Pixel) * Image.cols * Image.rows);
				while(pList != pList->next)  //进程遍历
				{
					get = false;
					while(!get)
					{
						pList = pList->next;
						i = pList->next->BlockIdx;
						MPI_Test(&handle[i], &get, &status);
					}
					ProcessList = pList->next;
					pList->next = ProcessList->next;
					BlockFetch *iter = BlocksLists[ProcessList->BlockIdx];     //ProcessList->BlockIdx代表进程ID

			       //cout<<"copy数据，进程序号  "<<ProcessList->BlockIdx<<" 此时i为："<<i<<endl;
					

					Pixel *pBuffer = ImageBuffer + BufStart[ProcessList->BlockIdx];

				//	Pixel *pBuffer = ImageBuffer + BufStart[i];

					while(iter != NULL)
					{
						int id = iter->BlockIdx;
						if(EndX[id] > StartX[id] && EndY[id] > StartY[id] &&
						   StartX[id] >= 0 && StartY[id] >= 0) {
							Pixel *pImage = (Pixel*)Image.data + (StartY[id] * Image.cols + StartX[id]);
							int ColSize = EndX[id] - StartX[id];
							for(int i = StartY[id]; i < EndY[id]; i++) {
								memcpy(pImage, pBuffer, ColSize * sizeof(Pixel));
								pBuffer += ColSize;
								pImage += Image.cols;
							}

						}
						iter = iter->next;
					}
					delete ProcessList;
				}
				i = pList->BlockIdx;
				delete pList; get = false;
			    while(!get) MPI_Test(&handle[i], &get, &status);
				BlockFetch *iter = BlocksLists[i];
				Pixel *pBuffer = ImageBuffer + BufStart[i];


				while(iter != NULL)
				{
					int id = iter->BlockIdx;
					if(EndX[id] > StartX[id] && EndY[id] > StartY[id] &&
					   StartX[id] >= 0 && StartY[id] >= 0) 
					{
						Pixel *pImage = (Pixel*)Image.data + (StartY[id] * Image.cols + StartX[id]);
						int ColSize = EndX[id] - StartX[id];
						for(int i = StartY[id]; i < EndY[id]; i++)
						{
							memcpy(pImage, pBuffer, ColSize * sizeof(Pixel));
							pBuffer += ColSize;
							pImage += Image.cols;
						}
					}
					iter = iter->next;
				}

				delete[]handle;
				delete[]BufLen;
				delete[]BufStart;
				delete[]StartX; delete[]StartY; delete[]EndX; delete[]EndY;


			 resize(Image, ImageReturn, Size(ScreenWidth, ScreenHeight));

			imwrite("E:\\Workspace\\VS2010\\Result_data\\offline_store_image\\beforeResizesendimg2.png",Image);
			Mat m_imgGlobalLine = Mat(1, ScreenWidth * ScreenHeight, CV_8UC1);
			ImageReturn.reshape(1,1).copyTo(m_imgGlobalLine);

			 send(cflag, (char*)m_imgGlobalLine.data, ScreenWidth * ScreenHeight * sizeof(Pixel), 0);

			cout<<"send image over!!!"<<endl;

			cout<<"faSongTuPianCishu:  "<<jieshoucishu++<<endl;
			}
		}
		closesocket(cflag);
	}

	closesocket(sflag);
#ifdef _WINDOWS
	WSACleanup();
#endif
	return;
}
