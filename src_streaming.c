/* USAGE:
 * ./src_streaming path/to/input/wav targetSR
 */

#include "src.h"

#define buf_total_len 512  /* In samples. */
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
  drwav_uint64 inLen;
  float* input = drwav_open_file_and_read_pcm_frames_f32(inFile, &channels, &SR,
                                                         &inLen, NULL);
  if (input == NULL) {
    printf("Error while opening and reading WAV file!\n");
    return 1;
  }
  if (channels != 1) {
    printf("Error: only mono signals are currently supported!\n");
    return 1;
  }

  // Configure and perform the sample rate conversion
  double const irate = (double) SR;
  t_converter_config converterConfig = init_converter_config(irate, orate, 'h');
  const long outLen = get_output_length(inLen, converterConfig);
  float *output = (float*) calloc(outLen, sizeof(float));

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
        if (nRead + ilen < inLen) {
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
  drwav_free(input, NULL);
  free(obuf);
  free(ibuf);
                                                              /* Diagnostics: */
  if (!!err) {
    fprintf(stderr, "%-26s %s; I/O: %s\n", argv[0], soxr_strerror(err),
        ferror(stdin) || ferror(stdout)? strerror(errno) : "no error");
    return !!err;
  }

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
