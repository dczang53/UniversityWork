// Header inclusions, if any...

#include <mpi.h>

#include "../lab1/gemm.h"

// Using declarations, if any...
#include <cstring>
//#include <iostream>

void GemmParallelBlocked(const float a[kI][kK], const float b[kK][kJ],
                         float c[kI][kJ]) {
  int RANK;
  int SIZE;   //# of processors assumed to be a power of 2
  MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
  MPI_Comm_size (MPI_COMM_WORLD, &SIZE);
  int COMM_WORLD_Partition_Rows = 1;
  int COMM_WORLD_Partition_Cols = 1;
  for(int i = SIZE; i > 1; i >>= 1)
  {
    if(COMM_WORLD_Partition_Rows <= COMM_WORLD_Partition_Cols)
      COMM_WORLD_Partition_Rows <<= 1;
    else
      COMM_WORLD_Partition_Cols <<= 1;
  }
  int ARows = kI / COMM_WORLD_Partition_Rows;
  int ARowRemainder = kI - (ARows * COMM_WORLD_Partition_Rows);
  int BRows = kJ / COMM_WORLD_Partition_Cols;
  int BRowRemainder = kJ - (BRows * COMM_WORLD_Partition_Cols);
  
  int rowColor = RANK / COMM_WORLD_Partition_Cols;
  int colColor = RANK % COMM_WORLD_Partition_Cols;
  MPI_Comm rowComm;
  MPI_Comm colComm;
  MPI_Comm_split(MPI_COMM_WORLD, rowColor, RANK, &rowComm);
  MPI_Comm_split(MPI_COMM_WORLD, colColor, RANK, &colComm);
  
  const int bufferARows = ((ARowRemainder != 0) && (rowColor == (COMM_WORLD_Partition_Rows - 1)))? ARowRemainder : ARows;
  const int bufferBRows = ((BRowRemainder != 0) && (colColor == (COMM_WORLD_Partition_Cols - 1)))? BRowRemainder : BRows;
  float *bufferA = (float*) malloc(bufferARows * kK * sizeof(float));
  float *bufferB = (float*) malloc(bufferBRows * kK * sizeof(float));
  float *bufferC = (float*) malloc(bufferARows * bufferBRows * sizeof(float));

  /*
  int bufsize;
  MPI_Pack_size(kI * kK, MPI_FLOAT, colComm, &bufsize);
  bufsize += kI * MPI_BSEND_OVERHEAD;
  float *fbuffer = (float*) malloc(bufsize * sizeof(float));
  MPI_Buffer_attach(fbuffer, bufsize);
  */
  if(RANK == 0)
  {
    for(int i = 0; i < kI; i++)
    {
      MPI_Request request;
      MPI_Isend(a[i], kK, MPI_FLOAT, (i / ARows), i, colComm, &request);
    }
    float *B = (float*) malloc(kJ * kK * sizeof(float));
    for(int i = 0; i < kK; i++)
      for(int j = 0; j < kJ; j++)
        B[(j * kK) + i] = b[i][j];
    for(int i = 0; i < kJ; i++)
    {
      MPI_Request request;
      MPI_Isend(&(B[i * kK]), kK, MPI_FLOAT, (i / BRows), i, rowComm, &request);
    }
  }
  if(colColor == 0)
  {
    int displ = rowColor * ARows;
    for(int i = 0; i < bufferARows; i++)
    {
      MPI_Request request;
      MPI_Irecv(&(bufferA[i * kK]), kK, MPI_FLOAT, 0, displ + i, colComm, &request);
      MPI_Status status;
      MPI_Wait(&request, &status);
      //MPI_Recv(&(bufferA[i]), kK, MPI_FLOAT, 0, 1, colComm, MPI_STATUS_IGNORE);
    }
  }
  if(rowColor == 0)
  {
    int displ = colColor * BRows;
    for(int i = 0; i < bufferBRows; i++)
    {
      MPI_Request request;
      MPI_Irecv(&(bufferB[i * kK]), kK, MPI_FLOAT, 0, displ + i, rowComm, &request);
      MPI_Status status;
      MPI_Wait(&request, &status);
      //MPI_Recv(&(bufferA[i]), kK, MPI_FLOAT, 0, 1, colComm, MPI_STATUS_IGNORE);
    }
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(bufferA, (bufferARows * kK), MPI_FLOAT, 0, rowComm);
  MPI_Bcast(bufferB, (bufferBRows * kK), MPI_FLOAT, 0, colComm);
  MPI_Barrier(MPI_COMM_WORLD);
  for(int i = 0; i < bufferARows; i++)
  {
    std::memset(&(bufferC[i * bufferBRows]), 0, sizeof(float) * bufferBRows);
    float *ARow = &(bufferA[i * kK]);
    float *CRow = &(bufferC[i * bufferBRows]);
    for(int j = 0; j < bufferBRows; j++)
    {
      float *BRow = &(bufferB[j * kK]);
      float Cbuf = 0;
      for(int k = 0; k < kK; k++)
        Cbuf += ARow[k] * BRow[k];
      CRow[j] += Cbuf;
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);

  float *RowBufferC;
  if(colColor == 0)
    RowBufferC = (float*) malloc(bufferARows * kJ * sizeof(float));
  for(int i = 0; i < bufferARows; i++)
    MPI_Gather(&(bufferC[i * bufferBRows]), bufferBRows, MPI_FLOAT, &(RowBufferC[i * kJ]), bufferBRows, MPI_FLOAT, 0, rowComm);
  //float *completeC = (float*) malloc(bufferARows * kJ * sizeof(float));
  if(colColor == 0)
  {
    int displ = rowColor * ARows;
    for(int i = 0; i < ARows; i++)
    {
      MPI_Request request;
      MPI_Isend(&(RowBufferC[i * kJ]), kJ, MPI_FLOAT, 0, displ + i, colComm, &request);
      //MPI_Status status;
      //MPI_Wait(&request, &status);
    }
  }
  if(RANK == 0)
  {
    for(int i = 0; i < kI; i++)
    {
      MPI_Request request;
      MPI_Irecv(c[i], kJ, MPI_FLOAT, (i / ARows), i, colComm, &request);
      MPI_Status status;
      MPI_Wait(&request, &status);
      //MPI_Recv(&(bufferA[i]), kK, MPI_FLOAT, 0, 1, colComm, MPI_STATUS_IGNORE);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
}
