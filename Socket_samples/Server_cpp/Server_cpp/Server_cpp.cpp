// Server_cpp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<WinSock2.h>
#include<iostream>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#define CONNECT_IP "166.111.65.211"
#define CONNECT_PORT 1000

int _tmain(int argc, _TCHAR* argv[])
{
	//创建套接字
	WORD myVersionRequest;
	WSADATA wsaData;
	myVersionRequest=MAKEWORD(1,1);
	int err;
	err=WSAStartup(myVersionRequest,&wsaData);
	if(!err)
	{
		std::cout<<"服务器已打开套接字"<<std::endl;
	}
	else
	{
		std::cout<<"服务器未打开套接字"<<std::endl;
	}
	//the parameter you need to bind
	SOCKET serSocket=socket(AF_INET,SOCK_STREAM,0);//创建了可识别套接字
	SOCKADDR_IN addr;
	addr.sin_family=AF_INET;
// 	addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
 	addr.sin_port=htons(CONNECT_PORT); //bind the port
	addr.sin_addr.S_un.S_addr=inet_addr(CONNECT_IP);
	bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));//finish binding
	listen(serSocket,5);//
	SOCKADDR_IN clientsocket;
	int len=sizeof(SOCKADDR);
	int times=0;	
	char receiveBuf[100];
	char sendBuf[100];
	memset(receiveBuf,0,sizeof(receiveBuf));
	memset(sendBuf,0,sizeof(sendBuf));
	while(1)
	{
		printf("waiting for connecting\n");
		SOCKET serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);
		if(serConn==INVALID_SOCKET)
		{
			printf("Fail to have a connection .  \n");
		}
		else
		{
			printf("Have a connection . IP is %s, Order is %d \n",inet_ntoa(clientsocket.sin_addr),++times);
		}
		int cnt=recv(serConn,receiveBuf,101,0);
			if(cnt<sizeof(int))
			{
				printf("server doesn't received the data from client ! the cnt is %d\n",cnt);
				closesocket(serConn);
				continue;
			}
			printf("The first time client send message is %s .the cnt is %d\n",receiveBuf,cnt);
			
			sprintf(sendBuf,"welcome %s to Beijing",inet_ntoa(clientsocket.sin_addr),"\n");fflush(stdout);
 			send(serConn,sendBuf,strlen(sendBuf)+1,0);
		while(true)
		{
			char receiveBuf1[100];
			memset(receiveBuf1,0,101);
			int cnt2=recv(serConn,receiveBuf1,101,0);
			printf("cnt2 is %d .sizeof(int) is %d\n",cnt2,sizeof(int));
			if(int(cnt2)<(int)(sizeof(int)))
			{
				closesocket(serConn);
				break;
			}
			++times;
			printf("The %d time client send message is %s\n",times,receiveBuf1);
			fflush(stdout);
			memset(receiveBuf,0,sizeof(receiveBuf));
		}
		closesocket(serConn);
		WSACleanup;
	}
	return 0;
}

