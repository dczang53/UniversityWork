// Header inclusions, if any...

#include "gemm.h"

// Using declarations, if any...
#include <cstring>

void GemmParallel(const float a[kI][kK], const float b[kK][kJ],
                  float c[kI][kJ]) {
  #pragma omp parallel for schedule(guided) num_threads(8)
  for (int i = 0; i < kI; ++i) {
    std::memset(c[i], 0, sizeof(float) * kJ);
    for (int k = 0; k < kK; ++k) {
      float aConst = a[i][k];
      const float *bCurr = b[k];
      float *cRow = c[i];
      for (int j = 0; j < kJ; ++j) {
        cRow[j] += aConst * *bCurr;
		bCurr++;
      }
    }
  }
}
