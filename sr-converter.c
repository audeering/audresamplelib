/* USAGE:
 * ./sr-converter path/to/input/wav targetSR
 */

#include <stdio.h>
#include <audsrc.h>
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
  unsigned long inLen = waveObj->nSamples;
  double irate = (double) waveObj->header.sampleRate;
  float *in = (float*) malloc(inLen * sizeof(float));
  read_wave(waveObj, in, 1, inLen, 1);
  free_wave(waveObj);

  size_t outLen = (size_t)(inLen * orate / irate + .5);   /* Array output len. */
  float * out = malloc(sizeof(*out) * outLen);       /* Allocate output buffer. */

  t_audsrc_config *audsrcConfig = init_audsrc_config(irate, orate);
  audsrc_oneshot(audsrcConfig, in, inLen, out, outLen);

  // Convert to integer PCM samples
  short signed int *out_uint16 = malloc(sizeof(short signed int) * outLen);
  for (int i = 0; i < outLen; ++i){
    out_uint16[i] = (short signed int) (FSR/2) * clipSample(out[i]);
  }

  // Pipe the audio data to ffmpeg, which writes it to a wav file
  FILE *pipeout;
  pipeout = popen("ffmpeg -y -f s16le -ar 16000 -ac 1 -i - output.wav", "w");
  fwrite(out_uint16, 2, outLen, pipeout);
  pclose(pipeout);

  free(in);
  free(out);
  free(out_uint16);
  return 0;
}
