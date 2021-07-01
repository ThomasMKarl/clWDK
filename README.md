# clWDK
This is an OpenCL program for examining complex zero points of polynomials.

Description: https://github.com/ThomasMKarl/clWDK/blob/master/doc/doc.pdf

## Dependencies:
 - an OpenMP and STD17 compliant C++ compiler
 - a valid OpenCL 3.0 platform and corresponding device (the program uses the first platform and first device it can find)
 - clang version 9 or higher
 - SPIRV-Tools
 - LLVM-SPIRV converter
 - AMD clRNG
 - GNU make
 - (optional) catch2 for unit tests
 - (optional) doxygen for code documentation
 - (optional) LuaLaTeX for additional information about the program

## Usage
Set CLRNG_PATH to clRNG root directory.

Edit Makefile.

Ensure that the compiler can find the paths to libOpenCL.so, libclRNG.so.

Ensure that the compiler can find the paths to CL/cl2.hpp and the clRNG headers.

make targets:

 all   : makes build, test and doc

 build : compiles executables

 test  : compiles and executes tests, creates example plots

 doc   : creates doxygen documentation and additional information

 clean : removes all generated files

 help  : prints help

executables:

"bin/wdk_host" is the CPU-only program.

"bin/wdk_ocl" is the GPU program.

Use the commands with inputs 

*"number of polynomials" "degree" "lower restriction for coefficients" "upper restriction for coefficients" "break condition(if(error < eps) break;)"*.

measure_time.sh contains an example simulation.

test/plot.py contains example plot scripts with numpy. It produces .pgf plots.

The result is a 1080*1080 pixel ppm image named zero_ocl.ppm. Maybe you want to convert it into a more compressed format e.g. with "convert zero_ocl.ppm zero_ocl.png".

