// Server_soc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<WinSock2.h>
#include<stdio.h>
#include<iostream>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#define CONNECT_IP "166.111.66.171"// "166.111.66.29"
typedef int socklen_t;
#define SOCKET_COM 5555
#define CONNECT_PORT 5555
#define SOCKET_BUFFER_LENGTH 32  //修改前256
using namespace std ;
 void main(int argc, _TCHAR* argv[])
{
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
	int times=0;
	while(true)
	{
		std::cout<<"Process  waiting for connection "<<std::endl;
		fflush(stdout);
		cflag=accept(sflag,(LPSOCKADDR)&remote,(socklen_t*)&addr_len);
		if(cflag==INVALID_SOCKET)
		{
			std::cout<<"cflag  invalid "<<cflag<<std::endl;
			isRun=false;
		}
		else
		{
			std::cout<<"cflag  true "<<cflag<<std::endl;
				printf("Have a connection . IP is %s, Order is %d \n",inet_ntoa(remote.sin_addr),++times);
			isRun=true;
		}
	}
	 
}

