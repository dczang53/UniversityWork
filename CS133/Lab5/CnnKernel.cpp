const int kNum = 256;
const int kKernel = 5;
const int kImSize = 224;
const int kInImSize = 228;
const int kOutImSize = 112;

#define max(X,Y) ((X)>(Y)?(X):(Y))

#pragma ACCEL kernel
void CnnKernel(
    const float input[kNum][kInImSize][kInImSize],
    const float weight[kNum][kNum][kKernel][kKernel], const float bias[kNum],
    float output[kNum][kOutImSize][kOutImSize]) {

  for(int i = 0; i < kNum; ++i)
  {
    float output_buf[kImSize][kImSize] = {};
    for(int j = 0; j < kNum; ++j)
    {
      float weight_buf[kKernel][kKernel];
      #pragma ACCEL parallel factor=5
      for(int pp = 0; pp < kKernel; ++pp) {
        #pragma ACCEL parallel factor=5
        for(int qq = 0; qq < kKernel; ++qq) {
          weight_buf[pp][qq] = weight[i][j][pp][qq];
        }
      }
      float input_buf[kInImSize][kInImSize + kKernel - 1][kKernel];
      //#pragma ACCEL pipeline
      for (int h = 0; h < kInImSize; ++h) {
        #pragma ACCEL parallel factor=6
        for (int w = 0; w < kInImSize; ++w) {
          #pragma ACCEL parallel factor=5
          for (int q = 0; q < kKernel; ++q) { //make kKernel copy of input(j,h,w)
            input_buf[h][w - q + kKernel - 1][q] = input[j][h][w];
          }
        }
      }

      //#pragma ACCEL pipeline
      for(int h = 0; h < kImSize; ++h)
      {
        #pragma ACCEL parallel factor=8
        for (int w = 0; w < kImSize; ++w)
        {
          float tmp = 0;
          #pragma ACCEL parallel factor=5
          for (int p = 0; p < kKernel; ++p) {
            #pragma ACCEL parallel factor=5
            for (int q = 0; q < kKernel; ++q) {
              tmp += weight_buf[p][q] * input_buf[h + p][w + kKernel - 1][q];
            }
          }
          output_buf[h][w] += tmp;
        }
      }
    }

    for(int h = 0; h < kOutImSize; ++h) {
      #pragma ACCEL parallel factor=16
      for(int w = 0; w < kOutImSize; ++w) {
        output[i][h][w] = max(  max(  max(output_buf[(h << 1)][(w << 1)] + bias[i], 0.0f), max(output_buf[(h << 1)][(w << 1) + 1] + bias[i], 0.0f)),
                                max(  max(output_buf[(h << 1) + 1][(w << 1)] + bias[i], 0.0f), max(output_buf[(h << 1) + 1][(w << 1) + 1] + bias[i], 0.0f)));
      }
    }
  }
}
