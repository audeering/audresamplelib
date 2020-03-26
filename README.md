# audresamplelib

A C wrapper around [soxr](https://sourceforge.net/projects/soxr/).


## Build
First, download, build and install `soxr` ([code](https://sourceforge.net/p/soxr/code/ci/master/tree/)).

In addition, clone, build and install the `audeering-libs` as a static library.
See [audering-libs](https://gitlab.audeering.com/tools/audeering-libs) (specifically, the section [Building and installing to a custom path](https://gitlab.audeering.com/tools/audeering-libs#building-and-installing-to-a-custom-path)).

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
(executables) for sample rate conversion, one based on the 
`audresample_oneshot()` function (`src_oneshot`), the other based on the `audresample_process()` function, which acts in a streaming fashion 
(`src_streaming`).

## Usage of the sample src tools
```
./src_oneshot <path/to/input/wav> <target-sr>
./src_streaming <path/to/input/wav> <target-sr>
```