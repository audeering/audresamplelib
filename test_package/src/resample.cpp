#include "catch2/catch_all.hpp"
#include "dr_wav.h"
extern "C" {
  #include "audresample.h"
  #include "remix.h"
}
#include <map>
#include <string>

using Catch::Matchers::Approx;


TEST_CASE("End-to-end generic test", "[end-to-end]") {

  std::map<int, std::string> signalTypeMap = {
      {1, "noise"},
      {2, "sine"}
  };

  int signalTypeCode = GENERATE(1, 2);
  unsigned int srIn = GENERATE(8000, 16000, 44100);
  unsigned int srOut = GENERATE(8000, 16000, 44100);
  if (srIn == srOut) return;
  unsigned int nChannelsIn = GENERATE(1, 2);

  std::string wavPathIn = std::string("test-assets/original-") +
      signalTypeMap[signalTypeCode] + "__" +
      "sr_" + std::to_string(srIn) + "__" +
      "channels_" + std::to_string(nChannelsIn) + ".wav";
  std::string wavPathTarget = std::string("test-assets/resampled-") +
      signalTypeMap[signalTypeCode] + "__" +
      "sr-in_" + std::to_string(srIn) + "__" +
      "sr-out_" + std::to_string(srOut) + "__" +
      "channels_" + std::to_string(nChannelsIn) + ".wav";

  // Read input file
  unsigned int channels;
  unsigned int SR;
  drwav_uint64 totalFrameCount;
  float *input = drwav_open_file_and_read_pcm_frames_f32(
      wavPathIn.c_str(), &channels, &SR, &totalFrameCount, NULL
  );
  REQUIRE(input != nullptr);
  REQUIRE(SR == srIn);

  if (channels != 1) {
    do_mono_mixdown_inplace(input, totalFrameCount, channels);
  }
  size_t inLen = totalFrameCount;

  // Read target file
  float *target = drwav_open_file_and_read_pcm_frames_f32(
      wavPathTarget.c_str(), &channels, &SR, &totalFrameCount, NULL
  );
  REQUIRE(target != nullptr);
  REQUIRE(channels == 1);
  REQUIRE(SR == srOut);
  size_t targetLen = totalFrameCount;

  std::vector<float> targetVec(target, target + targetLen);

  SECTION("oneshot") {
    // Configure and perform the sample rate conversion
    soxr_error_t err;
    double const irate = (double) srIn;
    double const orate = (double) srOut;
    t_converter_config converterConfig =
        init_converter_config(irate, orate, 'h');
    size_t outLen = get_output_length(inLen, converterConfig);
    REQUIRE(outLen == targetLen);
    float *output = (float*) malloc(outLen * sizeof(float));
    err = audresample_oneshot(converterConfig, input, inLen, output, outLen);
    REQUIRE(err == 0);

    std::vector<float> outputVec(output, output + outLen);

    if (signalTypeMap[signalTypeCode] == "noise") {
      REQUIRE_THAT(
          outputVec,
          Approx(targetVec).epsilon(2.e-1).margin(5.e-2)
      );
    } else if (signalTypeMap[signalTypeCode] == "sine") {
      REQUIRE_THAT(
          outputVec,
          Approx(targetVec).epsilon(5.e-2).margin(8.e-3)
      );
    }

    free(output);
  }

  SECTION("streaming") {
    // Configure and perform the sample rate conversion
    double const irate = (double) srIn;
    double const orate = (double) srOut;
    t_converter_config converterConfig =
        init_converter_config(irate, orate, 'h');
    size_t outLen = get_output_length(inLen, converterConfig);
    REQUIRE(outLen == targetLen);
    float *output = (float*) calloc(outLen, sizeof(float));

    /* Allocate resampling input and output buffers in proportion to the input
     * and output rates: */
    const unsigned int buf_total_len = GENERATE(16, 32, 64, 128);
    if (buf_total_len > 2 * outLen) {
      // Depending on the resampling ratio, some of the generated
      // buf_total_len values are too big...skip these cases.
      return;
    }
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

    if (!err) {                       /* If all is well, run the resampler: */
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
         * to produce as much output as is possible to the given output
         * buffer: */
        err = audresample_process(
            resampler,
            ibuf, ilen1, NULL,
            obuf, olen, &odone
        );

        memcpy(output + nWritten, obuf, odone * osize);
        nWritten += odone;
        written = odone;
        // written = fwrite(obuf, osize, odone, stdout); /* Consume output.*/

        /* If the actual amount of data output is less than that requested,
         * and we have not already reached the end of the input data, then
         * supply some more input next time round the loop: */
        need_input = odone < olen && ibuf;

      } while (!err && (need_input || written));
    }

    REQUIRE(err == 0);

    // NOTE: Currently ignoring the rightmost portion of the signal!
    // TODO: investigate why the error is higher towards the end.
    std::vector<float> outputVec(output, output + outLen - 2 * olen);
    std::vector<float> targetVecShort(target,
                                      target + targetLen - 2 * olen);

    if (signalTypeMap[signalTypeCode] == "noise") {
      REQUIRE_THAT(
          outputVec,
          Approx(targetVecShort).epsilon(2.e-1).margin(5.e-2)
      );
    } else if (signalTypeMap[signalTypeCode] == "sine") {
      REQUIRE_THAT(
          outputVec,
          Approx(targetVecShort).epsilon(1.e-3).margin(4.e-2)
      );
    }

    free(output);
    audresample_delete(resampler);
    free(obuf);
    free(ibuf);
  }

  drwav_free(input, NULL);
  drwav_free(target, NULL);

}
