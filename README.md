# audresamplelib

A C wrapper around [soxr](https://sourceforge.net/projects/soxr/) for Sample 
Rate Conversion (SRC).


## Build
First, download, build and install `soxr` ([code](https://sourceforge.net/p/soxr/code/ci/master/tree/)).

In addition, clone, build and install the `audeering-libs` as a static library.
See [audering-libs](https://gitlab.audeering.com/tools/audeering-libs) 
(specifically, the section 
[Building and installing to a custom path](https://gitlab.audeering.com/tools/audeering-libs#building-and-installing-to-a-custom-path)).

Then,
```bash
mkdir Release
cd Release
cmake \
    -DAUDEERING_LIBS_PATH=</path/to/audeering-libs> \
    -Daudeering_DIR=</path/to/audeering-libs>/lib/cmake/audeering \
    .. \
    -DCMAKE_BUILD_TYPE=Release
make
```
This will build both the wrapper library around `soxr` and two sample tools 
(executables) for SRC, one based on the `audresample_oneshot()` function 
(`src_oneshot`), the other based on the `audresample_process()` function, which 
acts in a streaming fashion (`src_streaming`).

## Usage examples
The two sample executables (`src_oneshot` and `src_streaming`) are not really 
meant for extensive use (even though they can be effectively used for SRC of wav
files), but their source code ([`src_oneshot.c`](src_oneshot.c) and 
[`src_streaming.c`](src_streaming.c)) is rather meant to serve as usage examples 
for the library's API. 

### Command line usage of the conversion tools
```
./src_oneshot <path/to/input/wav> <target-sr>
./src_streaming <path/to/input/wav> <target-sr>
```
As a result, a wav file will be written in the same folder as the input file, 
having the same name but with a suffix indicating the new sample rate, e.g., 
`my-wav-file.wav` -> `my-wav-file.wav_sr8000Hz.wav`.