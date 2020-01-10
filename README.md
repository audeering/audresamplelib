# audsrclib

A C wrapper around `soxr` (https://sourceforge.net/projects/soxr/).


## Build
First, download, build and install `soxr` (https://sourceforge.net/p/soxr/code/ci/master/tree/).

In addition, clone and build the `audeering-libs` as a static library (see 
https://gitlab.audeering.com/tools/audeering-libs). The `audeering-libs` are 
used here just for reading a wav file in the sample tool.  

Then,
```
mkdir Release
cd Release
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_LIBRARY_PATH="/path/to/audeering-libs;/usr/local/lib" \
    -DINCLUDE_DIRECTORIES="/path/to/audeering-libs/include;/usr/local/include"
make
```
This will build both the wrapper library around `soxr` and a sample executable 
that performs resampling.

## Usage of the sample executable
```
./sr-converter path/to/input/wav targetSR
```