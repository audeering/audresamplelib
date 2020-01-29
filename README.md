# audresamplelib

A C wrapper around `soxr` (https://sourceforge.net/projects/soxr/).


## Build
First, download, build and install `soxr` (https://sourceforge.net/p/soxr/code/ci/master/tree/).

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
This will build both the wrapper library around `soxr` and a sample executable 
that performs resampling.

## Usage of the sample executable
```
./sr-converter <path/to/input/wav> <target-sr>
```