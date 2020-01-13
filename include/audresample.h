#include <soxr.h>
#include <audeering/commonlib-audeering.h>

typedef struct {
  double srIn;
  double srOut;
} t_converter_config;

t_converter_config * init_converter_config(double srIn, double srOut);

void free_converter_config(t_converter_config *converterConfig);

size_t get_output_length(size_t inLen, t_converter_config *converterConfig);

void audresample_oneshot(
    t_converter_config *converterConfig,
    FLOAT_TYPE *in, size_t inLength,
    FLOAT_TYPE *out, size_t outLength
);
