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


//mpi_gatherv使用时，涉及到的mpi_gatherv进程必须都使用到。
//该例中 开启5个进程运行，0进程和1进程相当于向2,3进程发送数据。2,3进程进行接收数据。由于发送的地址是空地址，最后接收时由偏移量决定存储位置，
//?疑惑一是否有优化函数对其判断，因为在现在的程序中如果接收时先从进程1中接收3个数据量，再从进程0中接收数据量，则在接收的内存中进程一的部分数据会被刷新？
#include "mpi.h"
#include <stdio.h>
#define SIZE 27 
#define receive_start 1
#define receive_end   3
int main(int argc, char *argv[])
{
	int buffer[SIZE][SIZE],buffer1[SIZE][SIZE],buffer2[SIZE];
    int rank, size, i;
    int receive_counts[7] = { 1, 2, 0, 4,0,5,0 };

	int receive_counts_b[7]= { 2, 0, 0, 4,0,5,0 };
	int receive_count[7][7];
	int receive_displacement[7][7];

	int receive_displacementsb[7] = { 1, 2, 3, 4 ,7,11,16};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 	if((rank==0)||(rank==1))
    {
        buffer[0][0] = 0;
 		buffer[0][1] = 0;
        buffer[1][0] = 1;
        buffer[1][1] = 1;
		buffer[1][2] = 1;
		buffer[1][3] = 1;
        buffer[2][0] = 2;        
        buffer[2][1] = 2; 
        buffer[2][2] = 2; 
		buffer[3][0] = 3;
		buffer[3][1] = 3;
		buffer[3][2] = 3;
		buffer[3][3] = 3;
		
		buffer1[0][0] = 10;
        buffer1[1][0] = 11;
        buffer1[1][1] = 11;
        buffer1[2][0] = 12;        
        buffer1[2][1] = 12; 
        buffer1[2][2] = 12; 
		buffer1[3][0] = 13;
		buffer1[3][1] = 13;
		buffer1[3][2] = 13;
		buffer1[3][3] = 13;
		receive_count[0][0] = 1;
        receive_count[1][0] = 2;
        receive_count[2][0] = 3;        
		receive_count[3][0] = 4;
		receive_count[0][1] = 1;
        receive_count[1][1] = 2;
        receive_count[2][1] = 3;        
		receive_count[3][1] = 4;


    }
	else
	{


        receive_displacement[2][0] = 0;        
        receive_displacement[2][1] = 2; 

		receive_displacement[3][0] = 0;
		receive_displacement[3][1] = 3;
	}
	if((rank==0))  
   {
	   printf("\n\nrank is %d ,",rank);  
	   printf("size is %d\n\n",size);  
	   for(int i=receive_start;i<receive_end;i++)
	   {
		  printf("receive_counts is %d ,receive_counts[i] is %d, \n",receive_counts[i],receive_counts[i] );
		  printf("receive_displacement[i][rank] is %d\n\n\n",receive_displacement[i][rank]); 
 		  MPI_Gatherv((void*)buffer[i], receive_count[i][rank], MPI_INT, (void*)nullptr, receive_count[i], receive_displacement[i], MPI_INT, i+1, MPI_COMM_WORLD);
	   }
	}
	if((rank==1))
   {
	   printf("\n\nrank is %d ,",rank);  
	   printf("size is %d\n\n",size);  
	   for(int i=receive_start;i<receive_end;i++)
	   {
		  printf("i %d , receive_count[i][rank] is %d \n",i,receive_count[i][rank] );
		  printf("receive_displacement[i][rank] is %d\n\n\n",receive_displacement[i][rank]); 

    		  MPI_Gatherv((void*)buffer1[i], receive_count[i][rank], MPI_INT, (void*)nullptr, receive_count[i], receive_displacement[i], MPI_INT, i+1, MPI_COMM_WORLD);
	   }
	}
	 int receive_counts_a[7]= { 3, 3, 0, 0,0,0,0 };//receive_counts_a[0]需大于3
    if ((rank==3)||(rank == 2))
    {
		printf("rank is %d\n",rank);   
   		MPI_Gatherv(NULL, 0, MPI_INT, (void*)buffer2, receive_counts_a, receive_displacement[rank], MPI_INT, rank, MPI_COMM_WORLD);
 		for (i=0; i<SIZE; i++)
        {
            printf("[%d]", buffer2[i]);
        }
        printf("\n");
        fflush(stdout);

    }
 

 //   int buffer[16],buffer2[SIZE];
 //   int rank, size, i;
 //   int receive_counts[7] = { 1, 1, 2, 3,0,5,0 };
 //   int receive_displacements[7] = { 0, 1, 2, 4 ,7,11,16};
 //   MPI_Init(&argc, &argv);
 //   MPI_Comm_size(MPI_COMM_WORLD, &size);
 //   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 //   for (i=0; i<=rank; i++)
 //   {
 //       buffer[i] = rank;
 //   }
	//if((rank==1)||(rank==3)||(rank==0)||(rank==5) )
 //  {
	//   printf("rank is %d\n",rank);  printf("receive_counts is %d ,receive_displacements is %d\n",receive_counts[rank],receive_displacements[rank]);
	//   MPI_Gatherv((void*)buffer, receive_counts[rank], MPI_INT, (void*)nullptr, receive_counts, receive_displacements, MPI_INT, 2, MPI_COMM_WORLD);
	//}
 //   if (rank == 2)
 //   {
 //	  	 MPI_Gatherv(NULL, 0, MPI_INT, (void*)buffer2, receive_counts, receive_displacements, MPI_INT, rank, MPI_COMM_WORLD);
 //       for (i=0; i<SIZE; i++)
 //       {
 //           printf("[%d]", buffer2[i]);
 //       }
 //       printf("\n");
 //       fflush(stdout);
 //   }


   /*
       int receive_counts[6] = { 0, 1, 0, 3,4,5 };
    int receive_displacements[6] = { 0, 1, 2, 4 ,7,11};
	   int receive_3[6] = { 0, 0, 0, 0 ,0,0};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for (i=0; i<=rank; i++)
    {
        buffer[i] = rank;
    }
   if((rank==1)||(rank==3) )
   {
	   printf("rank is %d\n",rank);  printf("receive_counts is %d ,receive_displacements is %d\n",receive_counts[rank],receive_displacements[rank]);
	   MPI_Gatherv((void*)buffer, receive_counts[rank], MPI_INT, (void*)buffer2, receive_counts, receive_displacements, MPI_INT, 3, MPI_COMM_WORLD);
	
	}
    if (rank == 3)
    {
	  	 MPI_Gatherv(NULL, 0, MPI_INT, (void*)buffer2, receive_3, receive_displacements, MPI_INT, rank, MPI_COMM_WORLD);
        for (i=0; i<SIZE; i++)
        {
            printf("[%d]", buffer2[i]);
        }
        printf("\n");
        fflush(stdout);
    }*/


	printf("\n\nHello rank %d\n\n",rank);
    MPI_Finalize();
    return 0;
}