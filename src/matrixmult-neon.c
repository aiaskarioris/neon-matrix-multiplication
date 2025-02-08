#include "header.h"

inline void clearOutputArea8(int8_t *area, size_t dim);
inline void clearOutputArea16(int16_t *area, size_t dim);
inline void clearOutputArea32(int32_t *area, size_t dim);
//inline void clearOutputArea16f(float16_t *area, size_t dim);
inline void clearOutputArea32f(float32_t *area, size_t dim);

// In this implementation, each row of the second matrix (in2) is loaded
// once into a vector register. While a row is loaded, all computations
// using the row are executed before the next row is loaded.
#define VREG_T  int32x4_t
#define VREG_S  4
#define VLD1_F  vld1q_s32
#define VST1_F  vst1q_s32
#define VMAC_F  vmlaq_s32
#define CLEAROUTPUT_F   clearOutputArea32
#define LOADREPEATING_F vld1q_dup_s32
void matrixMultIRF32(int32_t *in1, int32_t *in2, int32_t *out0, size_t dim) {
    size_t currentRow, currentColElem;

    // Index of first element of row `in2[]`
    size_t in2PartIndex = 0;

    // Index of current element of current column in `in1[]`.
    // The current column of matrix1 has the same number as the current row
    // of matrix2
    size_t in1Index = 0;
    size_t in1Column = 0;

    // Index of the first element of the current output row
    size_t outVecStartIndex = 0;

    // Vector registers
    VREG_T in1ElemVReg, in2PartVReg, outPartVReg;

    // Clear the out0 area (this step might take a lot of time)
    CLEAROUTPUT_F(out0, dim);

    // Every row in `in2` is split into N VREG_T vectors (N=dim/VREF_S)
    // Each element in `in1` has to be multiplied with all elements of
    // a specific `in2` row. As such, when a VREG_T is loaded with a part
    // of an `in2` row, this part is multiplied with all elements of an `in1` column
    // and then this multiplication process must be repeated N times for the other
    // parts of the `in2` row.
    size_t partsInMat2Row = dim/VREG_S;
    size_t partOfRow = 0;

    // For every part of `in2`...
    for(in2PartIndex = 0; in2PartIndex < dim*dim; in2PartIndex += VREG_S) {
        // Load the current part to the vector register
        in2PartVReg = VLD1_F(&in2[in2PartIndex]);

        // Multiply with every element of the corresponding `in1` column
        for(in1Index = in1Column; in1Index < dim*dim; in1Index += dim) {
            // Load this element into the vector register
            in1ElemVReg = LOADREPEATING_F(&in1[in1Index]);

            // Load the output vector
            outPartVReg = VLD1_F(&out0[outVecStartIndex]);

            // outPartVReg += in1ElemVReg * in2PartVReg
            outPartVReg = VMAC_F(outPartVReg, in1ElemVReg, in2PartVReg);

            //printf("[in2[]: %2d-%2d\tin1[]: %2d\tout0[]: %2d-%2d\t]\n",
            //       in2PartIndex, in2PartIndex+VREG_S-1, in1Index, outVecStartIndex, outVecStartIndex+VREG_S-1);

            // Write result back to `out0`
            VST1_F(&out0[outVecStartIndex], outPartVReg);

            // Update indices
            // (Index for `in1` is updated by the loop)

            // `out0` index should move by one row
            outVecStartIndex += dim;
        }

        // Depending on where the next `in2` part is, `in1Column` and `out0VecStartIndex` should be
        // handled differently.
        partOfRow++;
        if(partOfRow == partsInMat2Row) { // Next part is on a new row
            in1Column++;
            outVecStartIndex = 0;
            partOfRow = 0;
        }
        else { // Next part is on the same row
            in1Column = in1Column;
            // `out0` index should be reset and point to the first row
            outVecStartIndex = partOfRow * VREG_S;
        }
    }
}

// === Int ==============================================================================================================================
#define IN_T    int16_t
#define VREG_T  int16x8_t
#define VREG_S  8
#define VLD1_F  vld1q_s16
#define VST1_F  vst1q_s16
#define VMAC_F  vmlaq_s16
#define CLEAROUTPUT_F   clearOutputArea16
#define LOADREPEATING_F vld1q_dup_s16
void matrixMultIRF16(IN_T *in1, IN_T *in2, IN_T *out0, size_t dim) {
    size_t currentRow, currentColElem;
    size_t in2PartIndex = 0;
    size_t in1Index = 0;
    size_t in1Column = 0;
    size_t outVecStartIndex = 0;

    VREG_T in1ElemVReg, in2PartVReg, outPartVReg;
    CLEAROUTPUT_F(out0, dim);

    size_t partsInMat2Row = dim/VREG_S;
    size_t partOfRow = 0;


    for(in2PartIndex = 0; in2PartIndex < dim*dim; in2PartIndex += VREG_S) {
        in2PartVReg = VLD1_F(&in2[in2PartIndex]);
        for(in1Index = in1Column; in1Index < dim*dim; in1Index += dim) {
            in1ElemVReg = LOADREPEATING_F(&in1[in1Index]);
            outPartVReg = VLD1_F(&out0[outVecStartIndex]);
            outPartVReg = VMAC_F(outPartVReg, in1ElemVReg, in2PartVReg);

            VST1_F(&out0[outVecStartIndex], outPartVReg);
            outVecStartIndex += dim;
        }
        partOfRow++;
        if(partOfRow == partsInMat2Row) {
            in1Column++;
            outVecStartIndex = 0;
            partOfRow = 0;
        }
        else {
            in1Column = in1Column;
            outVecStartIndex = partOfRow * VREG_S;
        }
    }
}

#define IN_T    int8_t
#define VREG_T  int8x16_t
#define VREG_S  16
#define VLD1_F  vld1q_s8
#define VST1_F  vst1q_s8
#define VMAC_F  vmlaq_s8
#define CLEAROUTPUT_F   clearOutputArea8
#define LOADREPEATING_F vld1q_dup_s8
void matrixMultIRF8(IN_T *in1, IN_T *in2, IN_T *out0, size_t dim) {
    size_t currentRow, currentColElem;
    size_t in2PartIndex = 0;
    size_t in1Index = 0;
    size_t in1Column = 0;
    size_t outVecStartIndex = 0;

    VREG_T in1ElemVReg, in2PartVReg, outPartVReg;
    CLEAROUTPUT_F(out0, dim);

    size_t partsInMat2Row = dim/VREG_S;
    size_t partOfRow = 0;

    for(in2PartIndex = 0; in2PartIndex < dim*dim; in2PartIndex += VREG_S) {
        in2PartVReg = VLD1_F(&in2[in2PartIndex]);
        for(in1Index = in1Column; in1Index < dim*dim; in1Index += dim) {
            in1ElemVReg = LOADREPEATING_F(&in1[in1Index]);
            outPartVReg = VLD1_F(&out0[outVecStartIndex]);
            outPartVReg = VMAC_F(outPartVReg, in1ElemVReg, in2PartVReg);

            VST1_F(&out0[outVecStartIndex], outPartVReg);
            outVecStartIndex += dim;
        }
        partOfRow++;
        if(partOfRow == partsInMat2Row) {
            in1Column++;
            outVecStartIndex = 0;
            partOfRow = 0;
        }
        else {
            in1Column = in1Column;
            outVecStartIndex = partOfRow * VREG_S;
        }
    }
}

// === Float =============================================================================================================================
#define IN_T    float32_t
#define VREG_T  float32x4_t
#define VREG_S  4
#define VLD1_F  vld1q_f32
#define VST1_F  vst1q_f32
#define VMAC_F  vmlaq_f32
#define CLEAROUTPUT_F   clearOutputArea32f
#define LOADREPEATING_F vld1q_dup_f32
void matrixMultIRF32f(IN_T *in1, IN_T *in2, IN_T *out0, size_t dim) {
    size_t currentRow, currentColElem;
    size_t in2PartIndex = 0;
    size_t in1Index = 0;
    size_t in1Column = 0;
    size_t outVecStartIndex = 0;

    VREG_T in1ElemVReg, in2PartVReg, outPartVReg;
    CLEAROUTPUT_F(out0, dim);

    size_t partsInMat2Row = dim/VREG_S;
    size_t partOfRow = 0;

    for(in2PartIndex = 0; in2PartIndex < dim*dim; in2PartIndex += VREG_S) {
        in2PartVReg = VLD1_F(&in2[in2PartIndex]);
        for(in1Index = in1Column; in1Index < dim*dim; in1Index += dim) {
            in1ElemVReg = LOADREPEATING_F(&in1[in1Index]);
            outPartVReg = VLD1_F(&out0[outVecStartIndex]);
            outPartVReg = VMAC_F(outPartVReg, in1ElemVReg, in2PartVReg);

            VST1_F(&out0[outVecStartIndex], outPartVReg);
            outVecStartIndex += dim;
        }
        partOfRow++;
        if(partOfRow == partsInMat2Row) {
            in1Column++;
            outVecStartIndex = 0;
            partOfRow = 0;
        }
        else {
            in1Column = in1Column;
            outVecStartIndex = partOfRow * VREG_S;
        }
    }
}


// ======================================================================================================================================
void clearOutputArea32(int32_t *area, size_t dim) {
    int32_t z = 0;
    int32x4_t   clrvreg = vld1q_dup_s32(&z);
    size_t i;
    for(i = 0; i < dim*dim; i+=4) { vst1q_s32(&area[i], clrvreg); }
}

void clearOutputArea16(int16_t *area, size_t dim) {
    int16_t z = 0;
    int16x8_t   clrvreg = vld1q_dup_s16(&z);
    size_t i;
    for(i = 0; i < dim*dim; i+=8) { vst1q_s16(&area[i], clrvreg); }
}

void clearOutputArea8(int8_t *area, size_t dim) {
    int8_t z = 0;
    int8x16_t   clrvreg = vld1q_dup_s8(&z);
    size_t i;
    for(i = 0; i < dim*dim; i+=16) { vst1q_s8(&area[i], clrvreg); }
}

void clearOutputArea32f(float32_t *area, size_t dim) {
    float32_t z = 0;
    float32x4_t   clrvreg = vld1q_dup_f32(&z);
    size_t i;
    for(i = 0; i < dim*dim; i+=4) { vst1q_f32(&area[i], clrvreg); }
}

void clearOutputArea16f(float16_t *area, size_t dim) {
    float16_t z = 0;
    float16x8_t   clrvreg = vld1q_dup_f16(&z);
    size_t i;
    for(i = 0; i < dim*dim; i+=8) { vst1q_f16(&area[i], clrvreg); }
}
