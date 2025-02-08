# Matrix Multiplication Benchmark Program for NEON ARM(R) Processors
This program executes tests on an ARMv8-A processor for matrix multiplication benchmarking.

## Algorithm
The algorithm for the multiplication is written with NEON intrinsics, which are C functions that get replaced with the appropriate NEON assembly instruction(s) by the compiler.

The main instructions used are `vld1q_dup` to broadcast a value into an SIMD register and `vmlaq` for multiply-accumulate.

While the code presented here is tested on square matrices with dimensions of multiple of 16, the algorithm could also work on matrices with arbitrary heights. The width of a matrix should be a multiple of the SIMD register size corresponding to each datatype (16 for 8 bits, 8 for 16 bits and 4 for 32 bits).

## Tests
For each supported data type, the program will execute a number of multiplications using increasing matrix sizes, namely 16x16, 32x32 and 48x48.

As this benchmark application is written for ARMv8-A CPUs, only `int8`, `int16`, `int32` and `fp32` types are tested. `fp16` NEON instructions are supported on the ARMv8.1-A architecture or later only.

## Results 
All tests shown where executed on an ARM(R) Cortex(TM)-A53 processor running PetaLinux. 

### 8-bit Integer

| **Matrix Size** | **Serial Exec. Time** (ms/iter.) | **NEON Exec. Time** (ms/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 21.96 | 1.96 | 11.2 |
| **32x32** | 173.82 | 15.15 | 11.5 |
| **48x48** | 5988.81 | 518.38 | 11.5 |

### 16-bit Integer

| **Matrix Size** | **Serial Exec. Time** (ms/iter.) | **NEON Exec. Time** (ms/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 253.15 | 42.75 | 5.5 |
| **32x32** | 2005.02 | 331.43 | 6.0 |
| **48x48** | 6748.17 | 1108.31 | 6.1 |

### 32-bit Integer

| **Matrix Size** | **Serial Exec. Time** (ms/iter.) | **NEON Exec. Time** (ms/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 228.94 | 85.31 | 2.7 |
| **32x32** | 1810.58 | 662.97 | 2.7 |
| **48x48** | 6095.35 | 2222.06 | 2.7 |

### 32-bit Float

| **Matrix Size** | **Serial Exec. Time** (ms/iter.) | **NEON Exec. Time** (ms/iter.) | **Speed-up** |
|-------------|------------------------------|----------------------------|----------|
| **16x16** | 229.00 | 89.69 | 2.5 |
| **32x32** | 1810.58 | 698.05 | 2.6 |
| **48x48** | 6094.83 | 2339.26 | 2.6 |
