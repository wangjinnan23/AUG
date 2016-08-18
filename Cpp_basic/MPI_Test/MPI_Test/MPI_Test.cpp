// MPI_Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
// #include <stdio.h>
// #include "mpi.h"
//#include<iostream>
// #define SIZE 20000
//
// int main( int argc, char *argv[])
// {
// int num_procs,my_id,flag;
// int buf[SIZE];
// int buf2[SIZE];
// for(int i=0;i<SIZE;i++)
// {
//	buf[i]=11*i;
//	buf2[i]=0;
// }
// MPI_Status status;
// MPI_Request handle;
//
// MPI_Init(&argc,&argv);
// MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
// MPI_Comm_rank(MPI_COMM_WORLD,&my_id);
//
// //两个进程，进程1非阻塞发送
// if ( my_id == 1 ) {
// MPI_Isend (buf, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &handle );
//
// flag = 0;
// //采用循环等待的方法等待非阻塞发送的完成
// while (flag == 0) {
// MPI_Test (&handle, &flag, &status);
// printf("%d Wait for completition flag = %d handle = %ld ....\n",
// my_id, flag, (long) handle);
// }
// }
// else if (my_id == 0 ) {
// //进程0阻塞接收
//	  int buf3[SIZE];
// MPI_Recv (buf3, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &status );
//  for(int i=0;i<10;i++)
// {
//	 
//	std::cout<<buf3[i];
//	 
// }
// }
//
// printf("%d Done ....\n",my_id);
//
// MPI_Finalize();
// return 0;
// }

//#include "mpi.h"
//#include <stdio.h>
//#include<iostream>
//#define size 100000
//int main(int argc, char *argv[])
//{
//	std::cout<<"Hello World is \n";
//    int myid, numprocs, left, right, flag=1;
//    int buffer[size], buffer2[size];
//		std::cout<<"\nflag is "<<flag<<"\n";
//    MPI_Request request;
//    MPI_Status status;
// 
//    MPI_Init(&argc,&argv);
//    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
//    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
// 
//	std::cout<<"Hello Kitty is \n";
// 
//	std::cout<<std::endl;
//	if(myid==1)
//	{
//		
//		MPI_Irecv(buffer2, 10, MPI_INT, 0, 123, MPI_COMM_WORLD, &request);
//		 
// 		MPI_Test(&request, &flag, &status);
//		std::cout<<"\nflag is "<<flag<<"\n";
//		for(int i=0;i<10;i++)
//			{
//				std::cout<<buffer2[i];
//			}
//		std::cout<<"\nDuring while \n\n";
//		while (!flag)
//		{
//        /* Do some work ... */
//			std::cout<<"\nflag is "<<flag<<"\n";
//			for(int i=0;i<10;i++)
//			{
//				std::cout<<buffer2[i];
//			}
//			MPI_Test(&request, &flag, &status);
//		}
//			std::cout<<"\nflag is "<<flag<<"\n";
//		for(int i=0;i<10;i++)
//			{
//				std::cout<<buffer2[i];
//			}
//		
//	}
//	if(myid==0)
//	{
//	for(int i=0;i<size;i++)
//	 {
//	 
//		buffer[i]=i;
//	 
//	 }
//		MPI_Send(buffer, 10, MPI_INT, 1, 123, MPI_COMM_WORLD);
//	}
//	
//	MPI_Finalize();
//    return 0;
//}

#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int buffer[16],buffer2[70];
    int rank, size, i;
    int receive_counts[6] = { 1, 1, 2, 3,4,5 };
    int receive_displacements[6] = { 0, 1, 2, 4 ,7,11};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //if (size != 4)
    //{
    //    if (rank == 0)
    //    {
    //        printf("Please run with 4 processes\n");fflush(stdout);
    //    }
    //    MPI_Finalize();
    //    return 0;
    //}
    for (i=0; i<=rank; i++)
    {
        buffer[i] = rank;
    }
// 	if((rank!=1)&&(rank!=0)&&(rank!=2))
 // 	if(rank!=3)
   {
	   printf("rank is %d\n",rank);
	   MPI_Gatherv(buffer, receive_counts[rank], MPI_INT, buffer2, receive_counts, receive_displacements, MPI_INT, 0, MPI_COMM_WORLD);
	}
    if (rank == 0)
    {
        for (i=0; i<70; i++)
        {
            printf("[%d]", buffer2[i]);
        }
        printf("\n");
        fflush(stdout);
    }
    MPI_Finalize();
    return 0;
}