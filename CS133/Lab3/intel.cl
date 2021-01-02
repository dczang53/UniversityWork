__constant int kNum = 256;
__constant int kKernel = 5;
__constant int kImSize = 224;
__constant int kInImSize = 228;
__constant int kOutImSize = 112;

__kernel
void CnnKernel(__constant float* input, __constant float* weight,
               __constant float* bias, __global float* output) {
  uint globalID = get_global_id(0);
  uint globalSize = get_global_size(0);  //assumes that globalSize is always a multiple of 2

  for (int i = globalID; i < kNum; i += globalSize) {
    for (int h = 0; h < kImSize; h += (kImSize / 16)) {
      __private float buffer[kImSize / 16][kImSize] = {};
      for (int j = 0; j < kNum; ++j) {
        float4 mask0 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + 0 + 0]);
        float maskRem0 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + 0 + 4];
        float4 mask1 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (1 * kKernel) + 0]);
        float maskRem1 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (1 * kKernel) + 4];
        float4 mask2 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (2 * kKernel) + 0]);
        float maskRem2 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (2 * kKernel) + 4];
        float4 mask3 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (3 * kKernel) + 0]);
        float maskRem3 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (3 * kKernel) + 4];
        float4 mask4 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (4 * kKernel) + 0]);
        float maskRem4 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (4 * kKernel) + 4];       
        for(int hh = 0; hh < (kImSize / 16); ++hh) {
          __constant float *rowEnd0 = &input[(j * kInImSize * kInImSize) + ((h + hh + 0) * kInImSize) + 4];
          __constant float *rowEnd1 = &input[(j * kInImSize * kInImSize) + ((h + hh + 1) * kInImSize) + 4];
          __constant float *rowEnd2 = &input[(j * kInImSize * kInImSize) + ((h + hh + 2) * kInImSize) + 4];
          __constant float *rowEnd3 = &input[(j * kInImSize * kInImSize) + ((h + hh + 3) * kInImSize) + 4];
          __constant float *rowEnd4 = &input[(j * kInImSize * kInImSize) + ((h + hh + 4) * kInImSize) + 4];
          float4 row0 = vload4(0, &input[(j * kInImSize * kInImSize) + ((h + hh + 0) * kInImSize) + 0]);
          float4 row1 = vload4(0, &input[(j * kInImSize * kInImSize) + ((h + hh + 1) * kInImSize) + 0]);
          float4 row2 = vload4(0, &input[(j * kInImSize * kInImSize) + ((h + hh + 2) * kInImSize) + 0]);
          float4 row3 = vload4(0, &input[(j * kInImSize * kInImSize) + ((h + hh + 3) * kInImSize) + 0]);
          float4 row4 = vload4(0, &input[(j * kInImSize * kInImSize) + ((h + hh + 4) * kInImSize) + 0]);
          float4 ans0 = mask0 * row0;
          float4 ans1 = mask1 * row1;
          float4 ans2 = mask2 * row2;
          float4 ans3 = mask3 * row3;
          float4 ans4 = mask4 * row4;
          float4 ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
          buffer[hh][0] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                         + (*rowEnd0 * maskRem0)
                         + (*rowEnd1 * maskRem1)
                         + (*rowEnd2 * maskRem2)
                         + (*rowEnd3 * maskRem3)
                         + (*rowEnd4 * maskRem4);
          for (int w = 1; w < kImSize; ++w) {
            row0 = (float4) (row0.s1, row0.s2, row0.s3, *rowEnd0);
            row1 = (float4) (row1.s1, row1.s2, row1.s3, *rowEnd1);
            row2 = (float4) (row2.s1, row2.s2, row2.s3, *rowEnd2);
            row3 = (float4) (row3.s1, row3.s2, row3.s3, *rowEnd3);
            row4 = (float4) (row4.s1, row4.s2, row4.s3, *rowEnd4);
            rowEnd0++;
            rowEnd1++;
            rowEnd2++;
            rowEnd3++;
            rowEnd4++;
            ans0 = mask0 * row0;
            ans1 = mask1 * row1;
            ans2 = mask2 * row2;
            ans3 = mask3 * row3;
            ans4 = mask4 * row4;
            ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
            buffer[hh][w] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                           + ((*rowEnd0) * maskRem0)
                           + ((*rowEnd1) * maskRem1)
                           + ((*rowEnd2) * maskRem2)
                           + ((*rowEnd3) * maskRem3)
                           + ((*rowEnd4) * maskRem4);
          }
        }
      }
      for(int m = 0; m < (kImSize / 16); m += 2) {
        for(int n = 0; n < kImSize; n += 16) {
          union {
            float F[16];
            float16 f;
          } localA;
          union {
            float F[16];
            float16 f;
          } localB;
          union {
            float F[16];
            float16 f;
          } localMax;
          localA.f = vload16(0, &buffer[m][n]);
          localB.f = vload16(0, &buffer[m + 1][n]);
          for(int y = 0; y < 16; ++y) {
            localA.F[y] = max(localA.F[y] + bias[i], 0.0f);
            localB.F[y] = max(localB.F[y] + bias[i], 0.0f);
          }
          localMax.f = max(localA.f, localB.f);
          for(int nn = 0; nn < 16; nn += 2)
            output[(i * kOutImSize * kOutImSize) + (((h + m) * kOutImSize) / 2) + ((n + nn) / 2)] = max(localMax.F[nn], localMax.F[nn + 1]);
        }
      }
    }
  }
}
