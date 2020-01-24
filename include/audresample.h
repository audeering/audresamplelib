#include <soxr.h>
#include <audeering/commonlib-audeering.h>
#include <stdlib.h>

typedef struct {
  double srIn;
  double srOut;
  char quality;
} t_converter_config;

/** The \p quality character is directly mapped to one of the 5 standard quality
 * recipes provided by soxr:
 * * q: quick
 * * l: low
 * * m: medium
 * * h: high
 * * v: very high */
soxr_quality_spec_t get_soxr_quality(char quality);

/** Configure the converter.
 *
 * Available characters for the \p quality argument:
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
