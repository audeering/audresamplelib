/* USAGE:
 * ./src_oneshot path/to/input/wav targetSR
 */


#include "src.h"

#define STRBUFFERLEN 1024


int main(int argc, char **argv)
{
  const char *inFile = argv[1];
  double const orate = atof(argv[2]);

  // Prepare output file name
  char outFile[STRBUFFERLEN];
  int outFileLen = snprintf(outFile, STRBUFFERLEN,
                            "%s_sr%sHz.wav", argv[1], argv[2]);
  if (outFileLen > STRBUFFERLEN) {
    printf("Input file name is too long!\n");
    return 1;
  }

  // Read input file
  unsigned int channels;
  unsigned int SR;
  drwav_uint64 totalFrameCount;
  float* input = drwav_open_file_and_read_pcm_frames_f32(
      inFile, &channels, &SR, &totalFrameCount, NULL
  );
  if (input == NULL) {
    printf("Error while opening and reading WAV file!\n");
    return 1;
  }
  if (channels != 1) {
    do_mono_mixdown_inplace(input, totalFrameCount, channels);
  }
  size_t inLen = totalFrameCount;

  // Configure and perform the sample rate conversion
  soxr_error_t err;
  double const irate = (double) SR;
  t_converter_config converterConfig = init_converter_config(irate, orate, 'h');
  size_t outLen = get_output_length(inLen, converterConfig);
  float *output = malloc(outLen * sizeof(float));
  err = audresample_oneshot(converterConfig, input, inLen, output, outLen);
  if (!!err) {
    printf("%-26s %s\n", argv[0], soxr_strerror(err));
    return !!err;
  }
  drwav_free(input, NULL);

  // Convert from float to integer PCM samples
  short signed int *outputPCM =
      (short signed int*) calloc(outLen, sizeof(short signed int));
  for (int i = 0; i < outLen; ++i){
    outputPCM[i] = (short signed int) (FSR/2) * clipSample(output[i]);
  }
  free(output);

  // Write resampled signal to WAV
  drwav wavOut;
  drwav_data_format format;
  format.container = drwav_container_riff;  // normal WAV files
  format.format = DR_WAVE_FORMAT_PCM;
  format.channels = 1;
  format.sampleRate = (unsigned int) orate;
  format.bitsPerSample = 16;
  drwav_init_file_write(&wavOut, outFile, &format, NULL);
  drwav_uint64 framesWritten = drwav_write_pcm_frames(&wavOut, outLen,
                                                      outputPCM);
  if (framesWritten != outLen) {
    printf("Error: something went wrong while writing the output wav file!\n");
    return 1;
  }
  drwav_uninit(&wavOut);
  free(outputPCM);

  return 0;
}
