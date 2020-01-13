#include <soxr.h>
#include <audeering/commonlib-audeering.h>

typedef struct {
  double srIn;
  double srOut;
} t_audsrc_config;

t_audsrc_config * init_audsrc_config(double srIn, double srOut);

size_t get_output_length(size_t inLen, double srIn, double srOut);

void audsrc_oneshot(
    t_audsrc_config *audsrcConfig,
    FLOAT_TYPE *in, size_t inLength,
    FLOAT_TYPE *out, size_t outLength
);
