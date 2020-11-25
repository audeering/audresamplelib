#include <remix.h>

void do_mono_mixdown(
    float *outMono,
    const float *input,
    size_t totalFrameCount,
    unsigned int nChannels
) {
  if (nChannels == 1) return;
  float normFactor = 1.0 / (float) nChannels;
  for (size_t i = 0; i < totalFrameCount; ++i) {
    float acc = 0.0;
    for (unsigned int c = 0; c < nChannels; ++c) {
      acc += input[nChannels * i + c];
    }
    outMono[i] = normFactor * acc;
  }
}

void do_mono_mixdown_inplace(
    float *data,
    size_t totalFrameCount,
    unsigned int nChannels
) {
  if (nChannels == 1) return;
  float normFactor = 1.0 / (float) nChannels;
  for (size_t i = 0; i < totalFrameCount; ++i) {
    float acc = 0.0;
    for (unsigned int c = 0; c < nChannels; ++c) {
      acc += data[nChannels * i + c];
    }
    data[i] = normFactor * acc;
  }
  data = realloc(data, totalFrameCount * sizeof(float));
}
