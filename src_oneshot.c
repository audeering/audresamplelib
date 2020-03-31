/* USAGE:
 * ./src_oneshot path/to/input/wav targetSR
 */


#include "src.h"


int main(int argc, char **argv)
{
  const char *inFile = argv[1];
  double srOut = atof(argv[2]);

  // Prepare output file name
  const int strBufferLen = 1024;
  char outFile[strBufferLen];
  int outFileLen = snprintf(outFile, strBufferLen,
                            "%s_sr%sHz.wav", argv[1], argv[2]);
  if (outFileLen > strBufferLen) {
    printf("Input file name is too long!\n");
    return 1;
  }

  // Read input file
  t_wave_obj *waveObj = init_wave(inFile);
  unsigned long inLen = waveObj->nSamples;
  double srIn = (double) waveObj->header.sampleRate;
  FLOAT_TYPE *in = (FLOAT_TYPE*) malloc(inLen * sizeof(FLOAT_TYPE));
  read_wave(waveObj, in, 1, inLen, 1);
  free_wave(waveObj);

  // Configure and perform the sample rate conversion
  soxr_error_t err;
  t_converter_config converterConfig = init_converter_config(srIn, srOut, 'h');
  size_t outLen = get_output_length(inLen, converterConfig);
  FLOAT_TYPE *out = malloc(outLen * sizeof(FLOAT_TYPE));
  err = audresample_oneshot(converterConfig, in, inLen, out, outLen);
  if (!!err) {
    printf("%-26s %s\n", argv[0], soxr_strerror(err));
    return !!err;
  }

  // Convert from float to integer PCM samples
  short signed int *out_int16 = malloc(sizeof(short signed int) * outLen);
  for (int i = 0; i < outLen; ++i){
    out_int16[i] = (short signed int) (FSR/2) * clipSample(out[i]);
  }

  // Pipe the audio data to ffmpeg, which writes it the output file
  FILE *pipeout;
  char ffmpegCommand[strBufferLen];
  int commandLen = snprintf(
      ffmpegCommand, strBufferLen,
      "ffmpeg -y -f s16le -ar %s -ac 1 -i - %s", argv[2], outFile
  );
  if (commandLen > strBufferLen) {
    printf("Input file name is too long!\n");
    return 1;
  }
  pipeout = popen(ffmpegCommand, "w");
  fwrite(out_int16, 2, outLen, pipeout);
  pclose(pipeout);

  // Clean up
  free(in);
  free(out);
  free(out_int16);

  return 0;
}
