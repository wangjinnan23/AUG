// Client_cpp.cpp : �������̨Ӧ�ó������ڵ㡣
//���Ͷ˵�һ���͵ڶ�����Ҫ��ʱ

#include "stdafx.h"
#include<WinSock2.h>
#include<stdio.h>

#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#define CONNECT_IP "127.0.0.1"
#define CONNECT_PORT 2000
int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);
	if(!err)
	{
		printf("�ͻ����Ѵ�Ƕ����!\n");
	}
	else
	{
		printf("�ͻ���δ��Ƕ����!\n");
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
         printf("���ӳɹ�!\n");
     }	
 
	char sendBuf[32]="1 1 0 4";
	

	send(clientSocket,sendBuf,32,0);


	send(clientSocket,"hello,this is client",strlen("hello,this is client")+1,0);
 	//Sleep(1);
 
 

	send(clientSocket,"hello200",strlen("hello200")+1,0);

	

	send(clientSocket,sendBuf,101,0);
	

 
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
