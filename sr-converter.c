/* USAGE:
 * ./sr-converter path/to/input/wav targetSR
 */

#include <stdio.h>
#include <soxr.h>
#include <audeering/wavelib-audeering.h>
#include "sr-converter.h"

#define FSR 65536.0

float clipSample(float value)
{
  if (abs(value) > 1.0f) {
    value = value > 0.0 ? 1.0 : -1.0;
  }
  return value;
}


int main(int argc, char **argv)
{
  const char *in_file = argv[1];
  double orate = atof(argv[2]);

  t_wave_obj *waveObj = init_wave(in_file);
  unsigned long inputLength = waveObj->nSamples;
  double irate = (double) waveObj->header.sampleRate;
  float *in = (float*) malloc(inputLength * sizeof(float));
  read_wave(waveObj, in, 1, inputLength, 1);
  free_wave(waveObj);

  size_t olen = (size_t)(inputLength * orate / irate + .5);   /* Array output len. */
  float * out = malloc(sizeof(*out) * olen);       /* Allocate output buffer. */
  size_t odone;

  soxr_error_t error = soxr_oneshot(irate, orate, 1, /* Rates and # of chans. */
      in, inputLength, NULL,                         /* Input. */
      out, olen, &odone,                             /* Output. */
      NULL, NULL, NULL);                             /* Default configuration.*/
  printf("%-26s %s\n", argv[0], soxr_strerror(error));

  // Convert to integer PCM samples
  short signed int *out_uint16 = malloc(sizeof(short signed int) * olen);
  for (int i = 0; i < olen; ++i){
    out_uint16[i] = (short signed int) (FSR/2) * clipSample(out[i]);
  }

  // Pipe the audio data to ffmpeg, which writes it to a wav file
  FILE *pipeout;
  pipeout = popen("ffmpeg -y -f s16le -ar 16000 -ac 1 -i - output.wav", "w");
  fwrite(out_uint16, 2, olen, pipeout);
  pclose(pipeout);

  free(in);
  free(out);
  free(out_uint16);
  return !!error;
}
