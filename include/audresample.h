#include <soxr.h>
#include <audeering/commonlib-audeering.h>
#include <stdlib.h>

typedef struct {
  double srIn;
  double srOut;
  soxr_quality_spec_t quality;
} t_converter_config;


/** Configure the converter.
 *
 * The \p quality parameter is directly mapped to one of the 5 standard quality
 * recipes provided by soxr:
 * * q: quick
 * * l: low
 * * m: medium
 * * h: high
 * * v: very high */
t_converter_config init_converter_config(
    double srIn, double srOut, char quality
);

size_t get_output_length(size_t inLen, t_converter_config converterConfig);

soxr_io_spec_t to_sox_type(size_t dataSize);

void audresample_oneshot(
    t_converter_config converterConfig,
    FLOAT_TYPE *in, size_t inLength,
    FLOAT_TYPE *out, size_t outLength
);
