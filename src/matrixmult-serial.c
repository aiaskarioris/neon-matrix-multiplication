#include "header.h"

// Matrix multiplication for square matrices; Non-Neon SISD instructions are utilized.
// `dim` is the size of a row or column.
#define FUNC_T  int32_t
void serialMatrixMult32(int32_t *in1, int32_t *in2, int32_t *out0, size_t dim, int32_t *cache) {
    size_t row, col, i;

    for(col = 0; col < dim; col++) {
        // Store the current column in the cache array; It is probably faster
        // to do this once at the start of each column-loop than
        // figuring out where each element of a column is on the fly
        // every time it is required.
        for(i = 0; i < dim; i++) { cache[i] = in2[i*dim + col]; }

        // With a column of in2, for every row of in1...
        for(row = 0; row < dim; row++) {
            // The first partial of out[col][row] initializes the matrix
            out0[row*dim + col] = in1[row*dim] * cache[0];

            // Sum the other partials
            for(i = 1; i < dim; i++) {
                out0[row*dim + col] += in1[row*dim + i] * cache[i];
            }
        }
    }
}

// Matrix multiplication for square matrices; Non-Neon SISD instructions are utilized.
// `dim` is the size of a row or column.
#undef FUNC_T
#define FUNC_T  int16_t
void serialMatrixMult16(FUNC_T *in1, FUNC_T *in2, FUNC_T *out0, size_t dim, FUNC_T *cache) {
    size_t row, col, i;
    for(col = 0; col < dim; col++) {
        // Store the current column in the cache array; It is probably faster
        // to do this once at the start of each column-loop than
        // figuring out where each element of a column is on the fly
        // every time it is required.
        for(i = 0; i < dim; i++) { cache[i] = in2[i*dim + col];}

        // With this column of in2, for every row of in1...
        for(row = 0; row < dim; row++) {

            // The first partial of out[col][row] initializes the matrix
            out0[row*dim + col] = in1[row*dim] * cache[0];

            // Sum the other partials
            for(i = 1; i < dim; i++) {
                out0[row*dim + col] += in1[row*dim + i] * cache[i];
            }
        }
    }
}

// Matrix multiplication for square matrices; Non-Neon SISD instructions are utilized.
// `dim` is the size of a row or column.
#undef FUNC_T
#define FUNC_T  int8_t
void serialMatrixMult8(FUNC_T *in1, FUNC_T *in2, FUNC_T *out0, size_t dim, FUNC_T *cache) {
    size_t row, col, i;
    for(col = 0; col < dim; col++) {
        // Store the current column in the cache array; It is probably faster
        // to do this once at the start of each column-loop than
        // figuring out where each element of a column is on the fly
        // every time it is required.
        for(i = 0; i < dim; i++) { cache[i] = in2[i*dim + col];}

        // With this column of in2, for every row of in1...
        for(row = 0; row < dim; row++) {
            // The first partial of out[col][row] initializes the matrix
            out0[row*dim + col] = in1[row*dim] * cache[0];

            // Sum the other partials
            for(i = 1; i < dim; i++) {
                out0[row*dim + col] += in1[row*dim + i] * cache[i];
            }
        }
    }
}

#undef  FUNC_T
#define FUNC_T float32_t
void serialMatrixMult32f(FUNC_T *in1, FUNC_T *in2, FUNC_T *out0, size_t dim, FUNC_T *cache) {
    size_t row, col, i;
    for(col = 0; col < dim; col++) {
        // Store the current column in the cache array; It is probably faster
        // to do this once at the start of each column-loop than
        // figuring out where each element of a column is on the fly
        // every time it is required.
        for(i = 0; i < dim; i++) { cache[i] = in2[i*dim + col];}

        // With this column of in2, for every row of in1...
        for(row = 0; row < dim; row++) {
            // The first partial of out[col][row] initializes the matrix
            out0[row*dim + col] = in1[row*dim] * cache[0];

            // Sum the other partials
            for(i = 1; i < dim; i++) {
                out0[row*dim + col] += in1[row*dim + i] * cache[i];
            }
        }
    }
}
