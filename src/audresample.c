#include <audresample.h>

t_converter_config init_converter_config(double srIn, double srOut)
{
  t_converter_config config;
  config.srIn = srIn;
  config.srOut = srOut;
  return config;
}

size_t get_output_length(size_t inLen, t_converter_config converterConfig)
{
  double srIn = converterConfig.srIn;
  double srOut = converterConfig.srOut;
  size_t outLen = (size_t) (inLen * srOut / srIn + 0.5);
  return outLen;
}

soxr_io_spec_t to_sox_type(size_t dataSize)
{
  if (dataSize == sizeof(float)) {
    return soxr_io_spec(SOXR_FLOAT32, SOXR_FLOAT32);
  } else if (dataSize == sizeof(double)) {
    return soxr_io_spec(SOXR_FLOAT64, SOXR_FLOAT64);
  } else {
    exit(EXIT_FAILURE);
  }
}

void audresample_oneshot(
    t_converter_config converterConfig,
    FLOAT_TYPE *in, size_t inLen,
    FLOAT_TYPE *out, size_t outLen)
{
  size_t odone;
  soxr_io_spec_t ioSpec = to_sox_type(sizeof(FLOAT_TYPE));
  soxr_error_t error = soxr_oneshot(
      converterConfig.srIn, converterConfig.srOut, 1,  // Rates and # of chans
      in, inLen, NULL,                                 // Input
      out, outLen, &odone,                             // Output
      &ioSpec, NULL, NULL);                            // Configuration
}
