/***
3 functions:
* one that takes the output array as argument and writes to it,
* one that allocates the output array and returns it,
* one that calculates the size of the output array for a giving input size and sample rate.

a configuration struct to hold input and output SR
 */

#include <soxr.h>
#include <audeering/commonlib-audeering.h>

typedef struct {
  double srIn;
  double srOut;
} t_audsrc_config;

t_audsrc_config * init_audsrc_config(double srIn, double srOut);

void audsrc_oneshot(
    t_audsrc_config *audsrcConfig,
    FLOAT_TYPE *in, size_t inLength,
    FLOAT_TYPE *out, size_t outLength
);
