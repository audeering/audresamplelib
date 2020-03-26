/* USAGE:
 * ./src_streaming path/to/input/wav targetSR
 */

#include "src.h"

#define FSR 65536.0
#define buf_total_len 512  /* In samples. */


FLOAT_TYPE clipSample(FLOAT_TYPE value);

size_t read_block(
    FLOAT_TYPE *readPointer, FLOAT_TYPE *wholeSignal, size_t start,
    size_t blockSize
);


int main(int argc, char **argv)
{
  const char *inFile = argv[1];
  double const orate = atof(argv[2]);

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
  int SR = (int) waveObj->header.sampleRate;
  long nSamples = waveObj->nSamples;
  FLOAT_TYPE *input = (FLOAT_TYPE*) malloc(nSamples * sizeof(FLOAT_TYPE));
  read_wave(waveObj, input, 1, nSamples, 1);
  free_wave(waveObj);

  // Configure and perform the sample rate conversion
  double const irate = (double) SR;
  t_converter_config converterConfig = init_converter_config(irate, orate, 'h');
  const long nSamples_out = get_output_length(nSamples, converterConfig);
  FLOAT_TYPE *output = (FLOAT_TYPE*) calloc(nSamples_out, sizeof(FLOAT_TYPE));

  /* Allocate resampling input and output buffers in proportion to the input
   * and output rates: */
  size_t const olen = get_output_length_from_buffer_length(buf_total_len,
                                                           converterConfig);
  size_t const ilen = buf_total_len - olen;
  size_t const osize = sizeof(float), isize = osize;
  void * obuf = malloc(osize * olen);
  void * ibuf = malloc(isize * ilen);

  size_t odone, written, need_input = 1;
  soxr_error_t err;

  /* Create a stream resampler: */
  soxr_t resampler = audresample_create(converterConfig, &err);

  if (!err) {                         /* If all is well, run the resampler: */
    size_t nRead = 0;
    size_t nWritten = 0;
    do {
      size_t ilen1 = 0;

      if (need_input) {

        /* Read one block into the buffer, ready to be resampled: */
        if (nRead + ilen < nSamples) {
          memcpy(ibuf, input + nRead, ilen * isize);
          nRead += ilen;
          ilen1 = ilen;
        } else {
          ilen1 = 0;
        }

        // ilen1 = fread(ibuf, isize, ilen, stdin);

        if (!ilen1) {     /* If the is no (more) input data available, */
          free(ibuf);     /* set ibuf to NULL, to indicate end-of-input */
          ibuf = NULL;    /* to the resampler. */
        }
      }

      /* Copy data from the input buffer into the resampler, and resample
       * to produce as much output as is possible to the given output buffer: */
      err = audresample_process(
          resampler,
          ibuf, ilen1, NULL,
          obuf, olen, &odone
      );

      memcpy(output + nWritten, obuf, odone * osize);
      nWritten += odone;
      written = odone;
      // written = fwrite(obuf, osize, odone, stdout); /* Consume output.*/

      /* If the actual amount of data output is less than that requested, and
       * we have not already reached the end of the input data, then supply some
       * more input next time round the loop: */
      need_input = odone < olen && ibuf;

    } while (!err && (need_input || written));
  }
                                                                  /* Tidy up: */
  audresample_delete(resampler);
  free(input), free(obuf), free(ibuf);
                                                              /* Diagnostics: */
  if (!!err) {
    fprintf(stderr, "%-26s %s; I/O: %s\n", argv[0], soxr_strerror(err),
        ferror(stdin) || ferror(stdout)? strerror(errno) : "no error");
    return !!err;
  }

  // Convert from float to integer PCM samples
  long number_bytes_out = 2 * nSamples_out;
  short signed int *output_pcm =
      (short signed int*) calloc(nSamples_out, sizeof(short signed int));
  for (int i = 0; i < nSamples_out; ++i){
    output_pcm[i] = (short signed int) (FSR/2) * clipSample(output[i]);
  }
  free(output);

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
  fwrite(output_pcm, 2, nSamples_out, pipeout);
  pclose(pipeout);

  free(output_pcm);

  return 0;
}


FLOAT_TYPE clipSample(FLOAT_TYPE value)
{
  if (abs(value) > 1.0f) {
    value = value > 0.0 ? 1.0 : -1.0;
  }
  return value;
}
