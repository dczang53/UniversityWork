#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mpi.h>

#define k 4
#define N 10000
#define m 100

void hist(const int L[k * N], int h[m])
{
	int worldRank, worldSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	int partSize = (k * N) / worldSize;
	int partSizeRem = (((k * N) - 1) % partSize) + 1;
	int *LBuffer = new int[partSize];
	int *hBuffer = new int[m]();
	
	int *sendCount = new int[worldSize];
	int *displ = new int[worldSize];
	for(int i = 0, acc = 0; i < worldSize; i++)
	{
		if((i + 1) != worldSize)
			sendCount[i] = partSize;
		else
			sendCount[i] = partSizeRem;
		displ[i] = acc;
		acc += sendCount[i];
	}

	MPI_Scatterv(L, sendCount, displ, MPI_INT, LBuffer, sendCount[worldRank], MPI_INT, 0, MPI_COMM_WORLD);
	for(int i = 0; i < partSize; i++)
		hBuffer[LBuffer[i] - 1]++;
	MPI_Reduce(hBuffer, h, m, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	delete[] sendCount;
	delete[] displ;
	delete[] LBuffer;
	delete[] hBuffer;
}

int main(int argc, char** argv)
{
	int rank;
	int *L;
	int *h;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank == 0)
	{
		L = new int[k * N];
		srand(time(NULL));
		for(int i = 0; i < (k * N); i++)
			L[i] = (rand() % m) + 1;
		h = new int[m]();
	}
	MPI_Barrier(MPI_COMM_WORLD);
	hist(L, h);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		for(int i = 0; i < m; i++)
		{
			std::cout << (i + 1) << ":\t" << h[i] << std::endl;
		}
		delete[] L;
		delete[] h;
	}
	MPI_Finalize();
}
