// Client_cpp.cpp : 定义控制台应用程序的入口点。
//发送端第一个和第二个需要延时

#include "stdafx.h"
#include<WinSock2.h>
#include<stdio.h>

#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#define CONNECT_IP "166.111.66.171"// "166.111.66.29"
#define CONNECT_PORT 5555
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);
	if(!err)
	{
		printf("客户端已打开嵌套字!\n");
	}
	else
	{
		printf("客户端未打开嵌套字!\n");
		return 0;
	}
	SOCKET clientSocket=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr=inet_addr(CONNECT_IP);
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(CONNECT_PORT);


	int con_if=connect(clientSocket,(sockaddr*)&clientsock_in,sizeof(SOCKADDR));//
	if(con_if==SOCKET_ERROR)
	{
         printf("Connect Error\n");
         return -1;
     }
     else
     {
         printf("连接成功!\n");
     }	
//	char receiveBuf[100];
//	char sendBuf[100]="receiveBuf";char sendBuf1[100]="Bufbrbher Bufbrbher444";
//	 
//	send(clientSocket,"hello,this is client",strlen("hello,this is client")+1,0);
// 	//Sleep(1);
//	if(recv(clientSocket,receiveBuf,101,0)<sizeof(int))
//	{
//		printf("can't receiver the message!");
//	}
//	else
//	{
//			printf("%s\n",receiveBuf);
//	}
//
//	send(clientSocket,"hello200",strlen("hello200")+1,0);
//
//	
//
//	send(clientSocket,sendBuf,101,0);
//	
//	send(clientSocket,sendBuf1,101,0);
//
///*	if(recv(clientSocket,receiveBuf,101,0)<sizeof(int))
//	{
//		printf("can't receiver the message!");
//	}
//	else
//	{
//			printf("%s\n",receiveBuf);
//	}
//	send(clientSocket,receiveBuf,101,0);
//	recv(clientSocket,receiveBuf,101,0);
//	printf("%s\n",receiveBuf);*/
//	closesocket(clientSocket);
//	WSACleanup();
	return 0;
}

