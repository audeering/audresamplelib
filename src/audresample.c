#include <audresample.h>

t_converter_config * init_converter_config(double srIn, double srOut)
{
  t_converter_config *config =
      (t_converter_config*) malloc(sizeof(t_converter_config));
  config->srIn = srIn;
  config->srOut = srOut;
  return config;
}

void free_converter_config(t_converter_config *converterConfig)
{
  free(converterConfig);
}

size_t get_output_length(size_t inLen, t_converter_config *converterConfig)
{
  double srIn = converterConfig->srIn;
  double srOut = converterConfig->srOut;
  size_t outLen = (size_t) (inLen * srOut / srIn + 0.5);
  return outLen;
}

void audresample_oneshot(
    t_converter_config *converterConfig,
    FLOAT_TYPE *in, size_t inLen,
    FLOAT_TYPE *out, size_t outLen)
{
  size_t odone;
  soxr_error_t error = soxr_oneshot(
      converterConfig->srIn, converterConfig->srOut, 1,  // Rates and # of chans
      in, inLen, NULL,                                   // Input
      out, outLen, &odone,                               // Output
      NULL, NULL, NULL);                                 // Default config
}
