/* USAGE:
 * ./sr-converter path/to/input/wav targetSR
 */


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
  double srOut = atof(argv[2]);

  t_wave_obj *waveObj = init_wave(in_file);
  unsigned long inLen = waveObj->nSamples;
  double srIn = (double) waveObj->header.sampleRate;
  float *in = (float*) malloc(inLen * sizeof(float));
  read_wave(waveObj, in, 1, inLen, 1);
  free_wave(waveObj);

  size_t outLen = get_output_length(inLen, srIn, srOut);
  float * out = malloc(outLen * sizeof(float));

  // Configure and perform the conversion
  t_audsrc_config *audsrcConfig = init_audsrc_config(srIn, srOut);
  audsrc_oneshot(audsrcConfig, in, inLen, out, outLen);

  // Convert to integer PCM samples
  short signed int *out_int16 = malloc(sizeof(short signed int) * outLen);
  for (int i = 0; i < outLen; ++i){
    out_int16[i] = (short signed int) (FSR/2) * clipSample(out[i]);
  }

  // Pipe the audio data to ffmpeg, which writes it to a wav file
  FILE *pipeout;
  pipeout = popen("ffmpeg -y -f s16le -ar 16000 -ac 1 -i - output.wav", "w");
  fwrite(out_int16, 2, outLen, pipeout);
  pclose(pipeout);

  free(in);
  free(out);
  free(out_int16);
  return 0;
}
