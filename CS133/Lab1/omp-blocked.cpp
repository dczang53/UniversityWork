// Header inclusions, if any...

#include "gemm.h"
#include <cstring>
#include <cstdlib>
#define BLOCK_SIZE 256
//will work even if matrix size isn't perfect multiple of BLOCK_SIZE

//(fluctuates at 80 GFlops at block size of 128 and sample of 1024 to about 120GFlops at sample of 4096)
void GemmParallelBlocked(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]) {
  for (int i = 0; i < kI; ++i) {
    std::memset(c[i], 0, sizeof(float) * kJ);
  }
  for(int ai = 0; ai < kI; ai += BLOCK_SIZE) {
    int aiiMax = (ai + BLOCK_SIZE) > kI? (kI - ai) : BLOCK_SIZE;
    for(int k = 0; k < kK; k += BLOCK_SIZE) {
      int kkMax = (k + BLOCK_SIZE) > kK? (kK - k) : BLOCK_SIZE;
      float aCache[BLOCK_SIZE][BLOCK_SIZE];
      //float *aCache = (float *) malloc(aiiMax * kkMax * sizeof(float));
      #pragma omp parallel for schedule(guided) num_threads(8)
      for(int aii = 0; aii < aiiMax; ++aii) {
        for(int kk = 0; kk < kkMax; ++kk) {
          aCache[aii][kk] = a[ai + aii][k + kk];
          //aCache[(aii * kkMax) + kk] = a[ai + aii][k + kk];
        }
      }
      #pragma omp parallel for schedule(guided) num_threads(8)
      for(int bj = 0; bj < kJ; bj += BLOCK_SIZE) {
        int bjjMax = (bj + BLOCK_SIZE) > kJ? (kJ - bj) : BLOCK_SIZE;
        float bCache[BLOCK_SIZE][BLOCK_SIZE];
        //float *bCache = (float *) malloc(kkMax * bjjMax * sizeof(float));
        for(int kk = 0; kk < kkMax; ++kk) {
          for(int bjj = 0; bjj < bjjMax; ++bjj) {
            bCache[kk][bjj] = b[k + kk][bj + bjj];
            //bCache[(kk * bjjMax) + bjj] = b[k + kk][bj + bjj];
          }
        }
        for(int aii = 0; aii < aiiMax; ++aii) {
          for(int kk = 0; kk < kkMax; ++kk) {
            float aConst = aCache[aii][kk];
            float *bCacheRow = bCache[kk];
            for(int bjj = 0; bjj < bjjMax; ++bjj) {
              c[ai + aii][bj + bjj] += aConst * bCacheRow[bjj];
              //c[ai + aii][bj + bjj] += aCache[(aii * kkMax) + kk] * bCache[(kk * bjjMax) + bjj];
            }
          }
        }
        //free(bCache);
      }
      //free(aCache);
    }
  }
}

/*
////USING MALLOC (around 66GFlops at block size of 256 and sample of 1024)
void GemmParallelBlocked(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]) {
  for (int i = 0; i < kI; ++i) {
    std::memset(c[i], 0, sizeof(float) * kJ);
  }
  for (int ai = 0; ai < kI; ai += BLOCK_SIZE) {            //aBlock row
    for (int aj = 0; aj < kK; aj += BLOCK_SIZE) {          //aBlock column
      int aBlockRows = (kI + 1 - ai) < BLOCK_SIZE? (kI - ai) : BLOCK_SIZE;
      int aBlockCols = (kK + 1 - aj) < BLOCK_SIZE? (kK - aj) : BLOCK_SIZE;
      float *aBlock = (float *) malloc(aBlockRows * aBlockCols * sizeof(float));
      for (int aii = 0; aii < aBlockRows; ++aii) {
        for (int ajj = 0; ajj < aBlockCols; ++ajj) {
          aBlock[(aii * aBlockCols) + ajj] = a[ai + aii][aj + ajj];
        }
      }
      #pragma omp parallel for schedule(guided) num_threads(8)
      for (int bj = 0; bj < kJ; bj += BLOCK_SIZE) {          //bBlock column
        int bBlockCols = (kJ + 1 - bj) < BLOCK_SIZE? (kJ - bj) : BLOCK_SIZE;
        float *bBlock = (float *) malloc(aBlockCols * bBlockCols * sizeof(float));
        for (int bii = 0; bii < aBlockCols; ++bii) {
          for (int bjj = 0; bjj < bBlockCols; ++bjj) {
            bBlock[(bii * bBlockCols) + bjj] = b[aj + bii][bj + bjj];
          }
        }
        for(int x = 0; x < aBlockRows; ++x) {
          for(int k = 0; k < aBlockCols; ++k) {
            float aConst = aBlock[(x * aBlockCols) + k];
            const float *bRow = &bBlock[k * bBlockCols];
            for(int y = 0; y < bBlockCols; ++y) {
              c[ai + x][bj + y] += aConst * bRow[y];
            }
          }
        }
        free(bBlock);
      }
      free(aBlock);
    }
  }
}
*/
