#include <audsrc.h>

t_audsrc_config * init_audsrc_config(double srIn, double srOut)
{
  t_audsrc_config *config = (t_audsrc_config*) malloc(sizeof(t_audsrc_config));
  config->srIn = srIn;
  config->srOut = srOut;
  return config;
}

size_t get_output_length(size_t inLen, double srIn, double srOut)
{
  size_t outLen = (size_t) (inLen * srOut / srIn + 0.5);
  return outLen;
}

void audsrc_oneshot(
    t_audsrc_config *audsrcConfig,
    FLOAT_TYPE *in, size_t inLen,
    FLOAT_TYPE *out, size_t outLen)
{
  size_t odone;
  soxr_error_t error = soxr_oneshot(
      audsrcConfig->srIn, audsrcConfig->srOut, 1,    /* Rates and # of chans. */
      in, inLen, NULL,                               /* Input. */
      out, outLen, &odone,                           /* Output. */
      NULL, NULL, NULL);                             /* Default configuration.*/
}
