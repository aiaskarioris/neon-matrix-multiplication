#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arm_neon.h>

// === Block Generation ===========================================================================
void generateBlock8(int8_t *buffer, size_t count);
void generateBlock16(int16_t *buffer, size_t count);
void generateBlock32(int32_t *buffer, size_t count);
//void generateBlock16f(float16_t *buffer, size_t count);
void generateBlock32f(float32_t *buffer, size_t count);

// === Buffer Printing ============================================================================
void printMatrix8(int8_t *buffer, const char *buffername, size_t dim);
void printMatrix16(int16_t *buffer, const char *buffername, size_t dim);
void printMatrix32(int32_t *buffer, const char *buffername, size_t dim);
//void printMatrix16f(float16_t *buffer, const char *buffername, size_t dim);
void printMatrix32f(float32_t *buffer, const char *buffername, size_t dim);

// === Serial Matrix Multiplication ===============================================================
// Matrix multiplication for square matrices; Non-Neon, SISD instructions are utilized.
// `dim` is the size of a row or column.
// `cache` is a `dim` sized array used for extra calculations (better to allocate this outside the function)
void serialMatrixMult8(int8_t *in1, int8_t *in2, int8_t *out0, size_t dim, int8_t *cache);
void serialMatrixMult16(int16_t *in1, int16_t *in2, int16_t *out0, size_t dim, int16_t *cache);
void serialMatrixMult32(int32_t *in1, int32_t *in2, int32_t *out0, size_t dim, int32_t *cache);
//void serialMatrixMult16f(float16_t *in1, float16_t *in2, float16_t *out0, size_t dim, float16_t *cache);
void serialMatrixMult32f(float32_t *in1, float32_t *in2, float32_t *out0, size_t dim, float32_t *cache);

// === NEON Matrix Multiplication (Input Rows First) ==============================================
void matrixMultIRF8(int8_t *in1, int8_t *in2, int8_t *out0, size_t dim);
void matrixMultIRF16(int16_t *in1, int16_t *in2, int16_t *out0, size_t dim);
void matrixMultIRF32(int32_t *in1, int32_t *in2, int32_t *out0, size_t dim);
//void matrixMultIRF16f(float16_t *in1, float16_t *in2, float16_t *out0, size_t dim);
void matrixMultIRF32f(float32_t *in1, float32_t *in2, float32_t *out0, size_t dim);
