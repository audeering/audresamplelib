# audresamplelib

A C wrapper around [soxr](https://sourceforge.net/projects/soxr/) for Sample 
Rate Conversion (SRC).


## Build
The library is intended to be built via CMake, using Conan as a dependency manager. 
For the Conan part, it is suggested to set up and activate a Python virtual environment:

```bash
virtualenv --python=python3 venv
source venv/bin/activate
```

Once the virtual environment is activated, install the `conan` package:

```bash
pip install conan
```

Then,
```bash
mkdir Release
cd Release
conan install ..
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TOOLS=ON
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
for the library API. 

### Command line usage of the conversion tools
```
./src_oneshot <path/to/input/wav> <target-sr>
./src_streaming <path/to/input/wav> <target-sr>
```
As a result, a wav file will be written in the same folder as the input file, 
having the same name but with a suffix indicating the new sample rate, e.g., 
`my-wav-file.wav` -> `my-wav-file.wav_sr8000Hz.wav`.
