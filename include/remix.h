#include <stdlib.h>

/** Mono-mixdown of an arbitrary number of channels.
 *
 * Note: the output array (outMono) needs to be allocated externally (before
 * calling), setting its size to totalFrameCount.
 * Note: the multi-channel input is supposed to be characterised by an
 * interleaved channel representation, and to contain a number of elements equal
 * to nChannel * totalFrameCount. */
void do_mono_mixdown(
    float *outMono,          // OUT
    const float *input,      // IN
    size_t totalFrameCount,  // IN
    unsigned int nChannels   // IN
);


/** Mono-mixdown of an arbitrary number of channels (in-place).
 *
 * Note: the multi-channel input (data) is supposed to be characterised by an
 * interleaved channel representation, and to contain a number of elements equal
 * to nChannel * totalFrameCount.
 * Note: the data array is automatically reallocated to the target size
 * (totalFrameCount elements). */
void do_mono_mixdown_inplace(
    float *data,             // IN-OUT
    size_t totalFrameCount,  // IN
    unsigned int nChannels   // IN
);
