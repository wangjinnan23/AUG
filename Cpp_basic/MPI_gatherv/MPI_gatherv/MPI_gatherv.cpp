// MPI_gatherv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<mpi.h>
#include<iostream>
#include<stdint.h>

int _tmain(int argc, char* argv[])
{
	MPI_Comm comm; 
    int gsize,sendarray[100][150],*sptr; 
    int root, *rbuf, stride, myrank; 
    MPI_Datatype stype; 
    int *displs,i,*rcounts; 
 
 
    MPI_Comm_size( comm, &gsize); 
    MPI_Comm_rank( comm, &myrank ); 
    rbuf =	  (int *)malloc(gsize*stride*sizeof(int)); 
    displs =  (int *)malloc(gsize*sizeof(int)); 
    rcounts = (int *)malloc(gsize*sizeof(int)); 
    for (i=0; i<gsize; ++i) { 
        displs[i] = i*stride; 
        rcounts[i] = 100-i;     /* note change from previous example */ 
    } 
    /* Create datatype for the column we are sending 
     */ 
    MPI_Type_vector( 100-myrank, 1, 150, MPI_INT, &stype); 
    MPI_Type_commit( &stype ); 
    /* sptr is the address of start of "myrank" column 
     */ 
    sptr = &sendarray[0][myrank]; 
    MPI_Gatherv( sptr, 1, stype, rbuf, rcounts, displs, MPI_INT, 
                                                        root, comm); 
	MPI_Finalize();
	return 0;
}

