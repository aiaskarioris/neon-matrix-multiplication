# Matrix Multiplication Benchmark Program for NEON ARM(R) Processors
This program executes tests on an ARMv8-A processor for matrix multiplication benchmarking.

## Algorithm
The algorithm for the multiplication is written with NEON intrinsics, which are C functions that get replaced with the appropriate NEON assembly instruction(s) by the compiler.

The main instructions used are `vld1q_dup` to broadcast a value into an SIMD register and `vmlaq` for multiply-accumulate.

While the code presented here is tested on square matrices with dimensions of multiple of 16, the algorithm could also work on matrices with arbitrary heights. The width of a matrix should be a multiple of the SIMD register size corresponding to each datatype (16 for 8 bits, 8 for 16 bits and 4 for 32 bits).

## Tests
For each supported data type, the program will execute a number of multiplications using increasing matrix sizes, namely 16x16, 32x32 and 48x48. Each test was conducted multiple times (1024) and the measurements shown below are the mean times.

As this benchmark application is written for ARMv8-A CPUs, only `int8`, `int16`, `int32` and `fp32` types are tested. `fp16` NEON instructions are supported on the ARMv8.1-A architecture or later only.

## Results 
All tests shown where executed on an ARM(R) Cortex(TM)-A53 processor running PetaLinux. 

### 8-bit Integer

| **Matrix Size** | **Serial Exec. Time** (us/iter.) | **NEON Exec. Time** (us/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 21.96 | 1.96 | 11.2 |
| **32x32** | 173.82 | 15.15 | 11.5 |
| **48x48** | 584.81 | 50.62 | 11.5 |

### 16-bit Integer

| **Matrix Size** | **Serial Exec. Time** (us/iter.) | **NEON Exec. Time** (us/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 24.72 | 4.17 | 5.9 |
| **32x32** | 195.80 | 32.36 | 6.0 |
| **48x48** | 658.98 | 108.23 | 6.1 |

### 32-bit Integer

| **Matrix Size** | **Serial Exec. Time** (us/iter.) | **NEON Exec. Time** (us/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 22.36 | 8.33 | 2.7 |
| **32x32** | 176.81 | 64.74 | 2.7 |
| **48x48** | 595.19 | 216.90 | 2.7 |

### 32-bit Float

| **Matrix Size** | **Serial Exec. Time** (us/iter.) | **NEON Exec. Time** (us/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 22.36 | 8.76 | 2.5 |
| **32x32** | 176.81 | 68.71 | 2.6 |
| **48x48** | 595.19 | 228.45 | 2.6 |
