#include <audresample.h>

soxr_quality_spec_t get_soxr_quality(char quality)
{
  unsigned long qualityRecipe;
  switch (quality) {
    case 'q':
      qualityRecipe = SOXR_QQ;
      break;
    case 'l':
      qualityRecipe = SOXR_LQ;
      break;
    case 'm':
      qualityRecipe = SOXR_MQ;
      break;
    case 'h':
      qualityRecipe = SOXR_HQ;
      break;
    case 'v':
      qualityRecipe = SOXR_VHQ;
      break;
    default:
      qualityRecipe = SOXR_HQ;
  }
  // TODO: set the flags as well...
  soxr_quality_spec_t qualitySpec = soxr_quality_spec(qualityRecipe, 0);
  return qualitySpec;
}

t_converter_config init_converter_config(
    double srIn, double srOut, char quality)
{
  t_converter_config config;
  config.srIn = srIn;
  config.srOut = srOut;
  config.quality = quality;
  return config;
}

size_t get_output_length(size_t inLen, t_converter_config converterConfig)
{
  double srIn = converterConfig.srIn;
  double srOut = converterConfig.srOut;
  size_t outLen = (size_t) (inLen * srOut / srIn + 0.5);
  return outLen;
}

size_t get_output_length_from_buffer_length(
    size_t bufferLen, t_converter_config converterConfig)
{
  double srIn = converterConfig.srIn;
  double srOut = converterConfig.srOut;
  size_t outLen = (size_t) (srOut * bufferLen / (srIn + srOut) + .5);
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

soxr_error_t audresample_oneshot(
    t_converter_config converterConfig,
    const float *in, size_t inLen,
    float *out, size_t outLen)
{
  soxr_error_t err = 0;
  size_t odone;
  soxr_io_spec_t ioSpec = to_sox_type(sizeof(float));
  soxr_quality_spec_t qualitySpec = get_soxr_quality(converterConfig.quality);
  if (converterConfig.srIn != converterConfig.srOut) {
    err = soxr_oneshot(
        converterConfig.srIn, converterConfig.srOut, 1,  // Rates and # of chans
        in, inLen, NULL,                                 // Input
        out, outLen, &odone,                             // Output
        &ioSpec, &qualitySpec, NULL);                    // Configuration
  } else {
    memcpy(out, in, outLen * sizeof(float));
  }
  return err;
}

soxr_t audresample_create(
    t_converter_config converterConfig, soxr_error_t *error)
{
  soxr_io_spec_t ioSpec = to_sox_type(sizeof(float));
  soxr_quality_spec_t qualitySpec = get_soxr_quality(converterConfig.quality);
  soxr_t resampler = soxr_create(
      converterConfig.srIn,      /* Input sample-rate. */
      converterConfig.srOut,     /* Output sample-rate. */
      1,                         /* Number of channels to be used. */
      error,                     /* To report any error during creation. */
      &ioSpec,                   /* To specify non-default I/O formats. */
      &qualitySpec,              /* To specify non-default resampling quality.*/
      NULL);                     /* To specify non-default runtime resources.*/
  return resampler;
}

void audresample_delete(soxr_t resampler)
{
  soxr_delete(resampler);
}

soxr_error_t audresample_process(
    soxr_t resampler,
    const soxr_in_t in, size_t ilen, size_t *idone,
    soxr_out_t out, size_t olen, size_t *odone)
{
  soxr_error_t err = soxr_process(
      resampler,
      in, ilen, idone,
      out, olen, odone
  );
  return err;
}
