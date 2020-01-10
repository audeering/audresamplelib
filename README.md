# sr-converter-example

Example C project for a sample rate conversion tool based on `soxr` 
(https://sourceforge.net/projects/soxr/).


## Build
First, download, build and install `soxr` (https://sourceforge.net/p/soxr/code/ci/master/tree/).

In addition, clone and build the `audeering-libs` as a static library (see 
https://gitlab.audeering.com/tools/audeering-libs). 

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

## Usage
```
./sr-converter path/to/input/wav targetSR
```