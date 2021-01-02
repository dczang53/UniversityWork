__constant int kNum = 256;
__constant int kKernel = 5;
__constant int kImSize = 224;
__constant int kInImSize = 228;
__constant int kOutImSize = 112;

#define input(j, h, w) input[((j) * kInImSize * kInImSize + (h) * kInImSize + (w))]
#define weight(i, j, p, q) weight[((i) * kNum * kKernel * kKernel + (j) * kKernel * kKernel + (p) * kKernel + (q))]
#define output(i, h, w) output[((i) * kOutImSize * kOutImSize + (h) * kOutImSize + (w))]

__kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void CnnKernel(__constant float* input, __constant float* weight,
               __constant float* bias, __global float* output) {

  for(int i = 0; i < kNum; ++i)
  {
    float curr_bias = bias[i];
    float output_buf[kImSize][kImSize]
    __attribute__((xcl_array_partition(cyclic, 8, 2)))
    = {};
    for(int j = 0; j < kNum; ++j)
    {
      //load weight
      float4 mask0 = vload4(0, &(weight(i, j, 0, 0)));
      float maskRem0 = weight(i, j, 0, 4);
      float4 mask1 = vload4(0, &(weight(i, j, 1, 0)));
      float maskRem1 = weight(i, j, 1, 4);
      float4 mask2 = vload4(0, &(weight(i, j, 2, 0)));
      float maskRem2 = weight(i, j, 2, 4);
      float4 mask3 = vload4(0, &(weight(i, j, 3, 0)));
      float maskRem3 = weight(i, j, 3, 4);
      float4 mask4 = vload4(0, &(weight(i, j, 4, 0)));
      float maskRem4 = weight(i, j, 4, 4);

      float input_buf[kInImSize][kInImSize + 4][kKernel] //buffer of input
      __attribute__((xcl_array_partition(cyclic, 5, 1)))
      __attribute__((xcl_array_partition(cyclic, 8, 2)))
      __attribute__((xcl_array_partition(complete, 3)))
      ;
      //input load loop
      load_in:
        for (int h = 0; h < kInImSize; ++h) {
          __attribute__((opencl_unroll_hint(8)))
          __attribute__((xcl_pipeline_loop(1)))
          for (int w = 0; w < kInImSize; ++w) {
            input_buf[h][w + 4][0] = input(j, h, w);
            input_buf[h][w + 3][1] = input(j, h, w);
            input_buf[h][w + 2][2] = input(j, h, w);
            input_buf[h][w + 1][3] = input(j, h, w);
            input_buf[h][w][4] = input(j, h, w);
          }
        }
      //convolution loop
      conv:
      for(int h = 0; h < kImSize; ++h)
      {
        __attribute__((opencl_unroll_hint(8)))
        __attribute__((xcl_pipeline_loop))
        for (int w = 0; w < kImSize; ++w)
        {
          float rem0 = input_buf[h][w + 4][4] * maskRem0;
          float rem1 = input_buf[h + 1][w + 4][4] * maskRem1;
          float rem2 = input_buf[h + 2][w + 4][4] * maskRem2;
          float rem3 = input_buf[h + 3][w + 4][4] * maskRem3;
          float rem4 = input_buf[h + 4][w + 4][4] * maskRem4;
          float4 ansTotal = (mask0 * vload4(0, &(input_buf[h][w + 4][0])))
                          + (mask1 * vload4(0, &(input_buf[h + 1][w + 4][0])))
                          + (mask2 * vload4(0, &(input_buf[h + 2][w + 4][0])))
                          + (mask3 * vload4(0, &(input_buf[h + 3][w + 4][0])))
                          + (mask4 * vload4(0, &(input_buf[h + 4][w + 4][0])));
          output_buf[h][w] += (ansTotal.s0 + ansTotal.s1 + ansTotal.s2 + ansTotal.s3
                            + rem0 + rem1 + rem2 + rem3 + rem4);
        }
      }
    }
    //copy output here
    write:
      for(int h = 0; h < kOutImSize; ++h) {
        __attribute__((opencl_unroll_hint(7)))
        __attribute__((xcl_pipeline_loop))
        for(int w = 0; w < kOutImSize; ++w) {
          output(i, h, w) = max(  max(  max(output_buf[(h << 1)][(w << 1)] + bias[i], 0.0f), max(output_buf[(h << 1)][(w << 1) + 1] + bias[i], 0.0f)),
                                  max(  max(output_buf[(h << 1) + 1][(w << 1)] + bias[i], 0.0f), max(output_buf[(h << 1) + 1][(w << 1) + 1] + bias[i], 0.0f)));
        }
      }
  }
}
