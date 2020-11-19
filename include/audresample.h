#include <soxr.h>
#include <stdlib.h>
#include <string.h>


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


/** Compute the length of the output array (whole signal), given the length of
 * the input (whole signal). */
size_t get_output_length(size_t inLen, t_converter_config converterConfig);


/** Compute the length of the output array (single block in the streaming
 * framework), given the length of the working buffer.
 *
 * Note: the working buffer is designed to contain both the input and the output
 * blocks. */
size_t get_output_length_from_buffer_length(
    size_t bufferLen, t_converter_config converterConfig
);


/** Convert float data types to sox standard base don their size in bytes. */
soxr_io_spec_t to_sox_type(size_t dataSize);


/** Resample a block of data (oneshot fashion).
 *
 * Note: no need for creating a resampler object (just a converter
 * configuration object). */
soxr_error_t audresample_oneshot(
    t_converter_config converterConfig,
    const float *in, size_t inLength,
    float *out, size_t outLength
);


/** Allocate a resampler object (streaming fashion). */
soxr_t audresample_create(
    t_converter_config converterConfig, soxr_error_t *error
);


/** Free a resampler object (streaming fashion). */
void audresample_delete(soxr_t resampler);


/** Resample (streaming fashion).
 *
 * Note that no special meaning is associated with ilen or olen equal to zero.
 * End-of-input (i.e. no data is available nor shall be available) may be
 * indicated by setting `in' to NULL. */
soxr_error_t audresample_process(
    soxr_t resampler,      /* As returned by soxr_create. */
    const soxr_in_t in,    /* Input buffer, may be NULL (see above). */
    size_t ilen,           /* Input buf. length (samples per channel). */
    size_t *idone,         /* To return actual # samples used (<= ilen). */
    soxr_out_t out,        /* Output buffer.*/
    size_t olen,           /* Output buf. length (samples per channel). */
    size_t *odone          /* To return actual # samples out (<= olen). */
);
