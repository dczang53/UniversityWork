__constant int kNum = 256;
__constant int kKernel = 5;
__constant int kImSize = 224;
__constant int kInImSize = 228;
__constant int kOutImSize = 112;

__kernel
void CnnKernel(__global const float* input, __global const float* weight,
               __global const float* bias, __global float* output) {
  
  __private uint blockID = get_group_id(0);
  __private uint threadID = get_local_id(0);
  __private uint numBlocks = get_num_groups(0);
  __private uint numThreadsInBlock = get_local_size(0);
  
  __private int offset = (112 / numBlocks); // = 2
  __private int blockOffset = blockID * offset;
  
  for (int i = threadID; i < kNum; i += numThreadsInBlock)
  {
    __private float C[4][224] = {};   //2 * offset
    for (int j = 0; j < kNum; ++j)
    {
      __private float4 mask0 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + 0 + 0]);
      __private float maskRem0 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + 0 + 4];
      __private float4 mask1 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (1 * kKernel) + 0]);
      __private float maskRem1 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (1 * kKernel) + 4];
      __private float4 mask2 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (2 * kKernel) + 0]);
      __private float maskRem2 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (2 * kKernel) + 4];
      __private float4 mask3 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (3 * kKernel) + 0]);
      __private float maskRem3 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (3 * kKernel) + 4];
      __private float4 mask4 = vload4(0, &weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (4 * kKernel) + 0]);
      __private float maskRem4 = weight[(i * kNum * kKernel * kKernel) + (j * kKernel * kKernel) + (4 * kKernel) + 4];

      __local float inputBuffer[8][228];    //(2 * blockOffset) + 4
      barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
      for (int x = 0; x < 8; ++x)
        for(int y = threadID; y < kInImSize; y += numThreadsInBlock)
          inputBuffer[x][y] = input[(j * kInImSize * kInImSize) + (((blockOffset << 1) + x) * kInImSize) + y];
      barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

      __private float4 row0 = vload4(0, &inputBuffer[0][0]);
      __private float4 row1 = vload4(0, &inputBuffer[1][0]);
      __private float4 row2 = vload4(0, &inputBuffer[2][0]);
      __private float4 row3 = vload4(0, &inputBuffer[3][0]);
      __private float4 row4 = vload4(0, &inputBuffer[4][0]);
      __private float4 ans0 = mask0 * row0;
      __private float4 ans1 = mask1 * row1;
      __private float4 ans2 = mask2 * row2;
      __private float4 ans3 = mask3 * row3;
      __private float4 ans4 = mask4 * row4;
      __private float4 ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
      C[0][0] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                + (inputBuffer[0][4] * maskRem0)
                + (inputBuffer[1][4] * maskRem1)
                + (inputBuffer[2][4] * maskRem2)
                + (inputBuffer[3][4] * maskRem3)
                + (inputBuffer[4][4] * maskRem4);
      for (int w = 1; w < kImSize; ++w)
      {
        row0 = (float4) (row0.s1, row0.s2, row0.s3, inputBuffer[0][w + 3]);
        row1 = (float4) (row1.s1, row1.s2, row1.s3, inputBuffer[1][w + 3]);
        row2 = (float4) (row2.s1, row2.s2, row2.s3, inputBuffer[2][w + 3]);
        row3 = (float4) (row3.s1, row3.s2, row3.s3, inputBuffer[3][w + 3]);
        row4 = (float4) (row4.s1, row4.s2, row4.s3, inputBuffer[4][w + 3]);
        ans0 = mask0 * row0;
        ans1 = mask1 * row1;
        ans2 = mask2 * row2;
        ans3 = mask3 * row3;
        ans4 = mask4 * row4;
        ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
        C[0][w] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                      + (inputBuffer[0][w + 4] * maskRem0)
                      + (inputBuffer[1][w + 4] * maskRem1)
                      + (inputBuffer[2][w + 4] * maskRem2)
                      + (inputBuffer[3][w + 4] * maskRem3)
                      + (inputBuffer[4][w + 4] * maskRem4);
      }

      row0 = vload4(0, &inputBuffer[1][0]);
      row1 = vload4(0, &inputBuffer[2][0]);
      row2 = vload4(0, &inputBuffer[3][0]);
      row3 = vload4(0, &inputBuffer[4][0]);
      row4 = vload4(0, &inputBuffer[5][0]);
      ans0 = mask0 * row0;
      ans1 = mask1 * row1;
      ans2 = mask2 * row2;
      ans3 = mask3 * row3;
      ans4 = mask4 * row4;
      ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
      C[1][0] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                + (inputBuffer[1][4] * maskRem0)
                + (inputBuffer[2][4] * maskRem1)
                + (inputBuffer[3][4] * maskRem2)
                + (inputBuffer[4][4] * maskRem3)
                + (inputBuffer[5][4] * maskRem4);
      for (int w = 1; w < kImSize; ++w)
      {
        row0 = (float4) (row0.s1, row0.s2, row0.s3, inputBuffer[1][w + 3]);
        row1 = (float4) (row1.s1, row1.s2, row1.s3, inputBuffer[2][w + 3]);
        row2 = (float4) (row2.s1, row2.s2, row2.s3, inputBuffer[3][w + 3]);
        row3 = (float4) (row3.s1, row3.s2, row3.s3, inputBuffer[4][w + 3]);
        row4 = (float4) (row4.s1, row4.s2, row4.s3, inputBuffer[5][w + 3]);
        ans0 = mask0 * row0;
        ans1 = mask1 * row1;
        ans2 = mask2 * row2;
        ans3 = mask3 * row3;
        ans4 = mask4 * row4;
        ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
        C[1][w] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                      + (inputBuffer[1][w + 4] * maskRem0)
                      + (inputBuffer[2][w + 4] * maskRem1)
                      + (inputBuffer[3][w + 4] * maskRem2)
                      + (inputBuffer[4][w + 4] * maskRem3)
                      + (inputBuffer[5][w + 4] * maskRem4);
      }

      row0 = vload4(0, &inputBuffer[2][0]);
      row1 = vload4(0, &inputBuffer[3][0]);
      row2 = vload4(0, &inputBuffer[4][0]);
      row3 = vload4(0, &inputBuffer[5][0]);
      row4 = vload4(0, &inputBuffer[6][0]);
      ans0 = mask0 * row0;
      ans1 = mask1 * row1;
      ans2 = mask2 * row2;
      ans3 = mask3 * row3;
      ans4 = mask4 * row4;
      ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
      C[2][0] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                + (inputBuffer[2][4] * maskRem0)
                + (inputBuffer[3][4] * maskRem1)
                + (inputBuffer[4][4] * maskRem2)
                + (inputBuffer[5][4] * maskRem3)
                + (inputBuffer[6][4] * maskRem4);
      for (int w = 1; w < kImSize; ++w)
      {
        row0 = (float4) (row0.s1, row0.s2, row0.s3, inputBuffer[2][w + 3]);
        row1 = (float4) (row1.s1, row1.s2, row1.s3, inputBuffer[3][w + 3]);
        row2 = (float4) (row2.s1, row2.s2, row2.s3, inputBuffer[4][w + 3]);
        row3 = (float4) (row3.s1, row3.s2, row3.s3, inputBuffer[5][w + 3]);
        row4 = (float4) (row4.s1, row4.s2, row4.s3, inputBuffer[6][w + 3]);
        ans0 = mask0 * row0;
        ans1 = mask1 * row1;
        ans2 = mask2 * row2;
        ans3 = mask3 * row3;
        ans4 = mask4 * row4;
        ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
        C[2][w] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                      + (inputBuffer[2][w + 4] * maskRem0)
                      + (inputBuffer[3][w + 4] * maskRem1)
                      + (inputBuffer[4][w + 4] * maskRem2)
                      + (inputBuffer[5][w + 4] * maskRem3)
                      + (inputBuffer[6][w + 4] * maskRem4);
      }

      row0 = vload4(0, &inputBuffer[3][0]);
      row1 = vload4(0, &inputBuffer[4][0]);
      row2 = vload4(0, &inputBuffer[5][0]);
      row3 = vload4(0, &inputBuffer[6][0]);
      row4 = vload4(0, &inputBuffer[7][0]);
      ans0 = mask0 * row0;
      ans1 = mask1 * row1;
      ans2 = mask2 * row2;
      ans3 = mask3 * row3;
      ans4 = mask4 * row4;
      ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
      C[3][0] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                + (inputBuffer[3][4] * maskRem0)
                + (inputBuffer[4][4] * maskRem1)
                + (inputBuffer[5][4] * maskRem2)
                + (inputBuffer[6][4] * maskRem3)
                + (inputBuffer[7][4] * maskRem4);
      for (int w = 1; w < kImSize; ++w)
      {
        row0 = (float4) (row0.s1, row0.s2, row0.s3, inputBuffer[3][w + 3]);
        row1 = (float4) (row1.s1, row1.s2, row1.s3, inputBuffer[4][w + 3]);
        row2 = (float4) (row2.s1, row2.s2, row2.s3, inputBuffer[5][w + 3]);
        row3 = (float4) (row3.s1, row3.s2, row3.s3, inputBuffer[6][w + 3]);
        row4 = (float4) (row4.s1, row4.s2, row4.s3, inputBuffer[7][w + 3]);
        ans0 = mask0 * row0;
        ans1 = mask1 * row1;
        ans2 = mask2 * row2;
        ans3 = mask3 * row3;
        ans4 = mask4 * row4;
        ansTotal = ans0 + ans1 + ans2 + ans3 + ans4;
        C[3][w] += ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                      + (inputBuffer[3][w + 4] * maskRem0)
                      + (inputBuffer[4][w + 4] * maskRem1)
                      + (inputBuffer[5][w + 4] * maskRem2)
                      + (inputBuffer[6][w + 4] * maskRem3)
                      + (inputBuffer[7][w + 4] * maskRem4);
      }
    }
    for(int r = 0; r < offset; r++)
    {
      for (int c = 0; c < kOutImSize; ++c)
      {
        __private union {
          float F[4];
          float4 f;
        } buf;
        buf.F[0] = C[(r << 1)][(c << 1)] + bias[i];
        buf.F[1] = C[(r << 1)][(c << 1) + 1] + bias[i];
        buf.F[2] = C[(r << 1) + 1][(c << 1)] + bias[i];
        buf.F[3] = C[(r << 1) + 1][(c << 1) + 1] + bias[i];
        buf.f = max(buf.f, (float4) (0.0f, 0.0f, 0.0f, 0.0f));
        output[(i * kOutImSize * kOutImSize) + ((blockOffset + r) * kOutImSize) + c] = max(
                                                                                        max(buf.F[0], buf.F[1]),
                                                                                        max(buf.F[2], buf.F[3]));
      }
    }
  }
}

//STRAT: UNROLL LOOP h AS SHARED MEMORY inputBuffer SCALABLE
//HERE, "numBlocks" MUST BE SET AS 56
//~655 GFlops (56 x 256 = 14336)
