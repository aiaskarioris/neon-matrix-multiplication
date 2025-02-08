#include "header.h"
#include "clock.h"

typedef struct testResults_st {
  float32_t speedUp;
  float32_t usPerIter;
} testResults_t;

testResults_t int8Test();
testResults_t int16Test();
testResults_t int32Test();
testResults_t float32Test();

size_t dim;
size_t elementCount;
size_t iterations = 1024;

void *memstart, *in1, *in2, *out0;

int main(int argc, char **argv) {
  printf("Aias Karioris, 2024\n");
  printf("  Matrix Multiplication Test\n\n");

  // Memory for benchmarking
#define BENCHMARKTESTCOUNT 64
  uint32_t 	benchmarkDimension[BENCHMARKTESTCOUNT];
  float32_t benchmarkSpeedUp[BENCHMARKTESTCOUNT*4];
  float32_t benchmarkUSPerIt[BENCHMARKTESTCOUNT*4];
  uint8_t 	benchmarkTestCount = 0;
  testResults_t	results;

  size_t dims_to_test[] = {16, 32, 48};
  for(uint8_t didx = 0; didx < 6; didx++ ){
    dim = dims_to_test[didx];
    elementCount = dim*dim;

    benchmarkDimension[benchmarkTestCount] = dim;
    uint8_t m = 0;

    printf("\n# # # # # # # # # # # # # # # # # %2d x %2d Matrix Test # # # # # # # # # # # # # # # #\n", dim, dim);

    printf("\n### 32-bit Float Test #################################################################\n");
    results = float32Test();
    benchmarkSpeedUp[4*benchmarkTestCount] = results.speedUp;
    benchmarkUSPerIt[4*benchmarkTestCount] = results.usPerIter;

    printf("\n### 32-bit Integer Test ###############################################################\n");
    results = int32Test();
    benchmarkSpeedUp[4*benchmarkTestCount+1] = results.speedUp;
    benchmarkUSPerIt[4*benchmarkTestCount+1] = results.usPerIter;

    printf("\n### 16-bit Integer Test ###############################################################\n");
    if(dim < 8 || dim%8!=0) {
      benchmarkSpeedUp[4*benchmarkTestCount+2] = -1;
      benchmarkUSPerIt[4*benchmarkTestCount+2] = -1;
    }
    else {
        results = int16Test();
        benchmarkSpeedUp[4*benchmarkTestCount+2] = results.speedUp;
        benchmarkUSPerIt[4*benchmarkTestCount+2] = results.usPerIter;
    }

    printf("\n### 8-bit Integer Test ################################################################\n");
    if(dim < 16 || dim%16!=0) {
      benchmarkSpeedUp[4*benchmarkTestCount+3] = -1;
      benchmarkUSPerIt[4*benchmarkTestCount+3] = -1;
    }
    else {
        results = int8Test();
        benchmarkSpeedUp[4*benchmarkTestCount+3] = results.speedUp;
        benchmarkUSPerIt[4*benchmarkTestCount+3] = results.usPerIter;
    }

    benchmarkTestCount += 1;
  }


  printf("                                       Speed-Up\n"); printf("   Dimensions |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3dx%3d |", benchmarkDimension[i], benchmarkDimension[i]); }
  printf("\n 32-Bit Float |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" 0%3.2f   |", benchmarkSpeedUp[i*4]); }
  printf("\n 32-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" 0%3.2f   |", benchmarkSpeedUp[i*4+1]); }
  printf("\n 16-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" 0%3.2f   |", benchmarkSpeedUp[i*4+2]); }
  printf("\n  8-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3.2f   |", benchmarkSpeedUp[i*4+3]); }


  printf("\n\n                              Exec. Time / Iteration (us)\n"); printf("   Dimensions |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3dx%3d\t|", benchmarkDimension[i], benchmarkDimension[i]); }
  printf("\n 32-Bit Float |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3.2f\t|", benchmarkUSPerIt[i*4]); }
  printf("\n 32-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3.2f\t|", benchmarkUSPerIt[i*4+1]); }
  printf("\n 16-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3.2f\t|", benchmarkUSPerIt[i*4+2]); }
  printf("\n  8-Bit Int   |");
  for(uint8_t i = 0; i < benchmarkTestCount; i++) { printf(" %3.2f\t|", benchmarkUSPerIt[i*4+3]); }
  printf("\n[ OK ] Exiting normally.\n");  
  return 0;
}

#define TEST_T  int32_t
#define GENERATEBLOCK_F generateBlock32
#define PRINTMATRIX_F   printMatrix32
#define SERIALMULT_F    serialMatrixMult32
#define IRFMULT_F       matrixMultIRF32
testResults_t int32Test() {
  // Create `elementCount*iterations` pairs of input and space for one output.
  // More than 64 iterations will re-use blocks.
  size_t bufferCount = elementCount * (2 * (iterations > 64?64:iterations) + 1);

  // Allocate memory for numbers
  memstart = malloc(sizeof(TEST_T) * (size_t)bufferCount);
  printf("[INFO] Allocated memory at 0x%4x\n", memstart);
  if(memstart == NULL) {
    printf("[ERR ] Memory allocation error! Exiting...\n");
    testResults_t err = {-1, -1};
    return err;
  }

  // out0 starts at the last dim*dim block
  out0 = memstart + (sizeof(TEST_T) * elementCount * 2 * (iterations > 64?64:iterations));

  // Populate
  startClock();
  srand(clock());
  GENERATEBLOCK_F(memstart, elementCount * 2 * (iterations>64?64:iterations));
  printf("[ OK ] Populated buffer(s) in %3.1fms.\n", clockToMS(readClock()));

  // Variables for testing
  float32_t execTimeSerial, execTimePIRF;

  // Serial Multiplication - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_T *temparray = (TEST_T*)malloc(dim * sizeof(TEST_T));
  size_t it;

  // These pointers are for more readability
  size_t blockSize = sizeof(TEST_T) * elementCount;
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    SERIALMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim, temparray);
  }

  clock_t endTime = readClock();
  execTimeSerial = clockToMS(endTime);

  printf("[DONE] Serial Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));


  // NEON Mult. (Input 2 Row First) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    IRFMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim);
  }
  endTime = readClock();
  execTimePIRF = clockToMS(endTime);

  printf("[DONE] P-IRF Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  free(memstart);
  free(temparray);

  testResults_t results;
  results.speedUp   = execTimeSerial / execTimePIRF;
  results.usPerIter = clockToMS((float32_t)endTime*100.0/(float32_t)iterations);

  return results;
}


#define TEST_T  int16_t
#define GENERATEBLOCK_F generateBlock16
#define PRINTMATRIX_F   printMatrix16
#define SERIALMULT_F    serialMatrixMult16
#define IRFMULT_F       matrixMultIRF16
testResults_t int16Test() {
  // Create elementCount*iterations pairs of input and space for one output.
  // More than 64 iterations will re-use blocks.
  size_t bufferCount = elementCount * (2 * (iterations > 64?64:iterations) + 1);

  // Allocate memory for numbers
  memstart = malloc(sizeof(TEST_T) * (size_t)bufferCount);
  printf("[INFO] Allocated memory at 0x%4x\n", memstart);
  if(memstart == NULL) {
    printf("[ERR ] Memory allocation error! Exiting...\n");
        testResults_t err = {-1, -1};
    return err;
  }

  // out0 starts at the last dim*dim block
  out0 = memstart + (sizeof(TEST_T) * elementCount * 2 * (iterations > 64?64:iterations));

  // Populate
  startClock();
  srand(clock());
  GENERATEBLOCK_F(memstart, elementCount * 2 * (iterations>64?64:iterations));

  // Variables for testing
  float32_t execTimeSerial, execTimePIRF;

  // Serial Multiplication - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_T *temparray = (TEST_T*)malloc(dim * sizeof(TEST_T));
  size_t it;

  // Indexing blocks should be done as simply as possible...
  size_t blockSize = sizeof(TEST_T) * elementCount;
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    SERIALMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim, temparray);
  }

  clock_t endTime = readClock();
  execTimeSerial = clockToMS(endTime);

  printf("[DONE] Serial Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  // NEON Mult. (Input 2 Row First) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    IRFMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim);
  }
  endTime = readClock();
  execTimePIRF = clockToMS(endTime);

  printf("[DONE] P-IRF Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  free(memstart);
  free(temparray);

  testResults_t results;
  results.speedUp   = execTimeSerial / execTimePIRF;
  results.usPerIter = clockToMS((float32_t)endTime*100.0/(float32_t)iterations);

  return results;
}

#define TEST_T  int8_t
#define GENERATEBLOCK_F generateBlock8
#define PRINTMATRIX_F   printMatrix8
#define SERIALMULT_F    serialMatrixMult8
#define IRFMULT_F       matrixMultIRF8
testResults_t int8Test() {
  // Create elementCount*iterations pairs of input and space for one output.
  // More than 64 iterations will re-use blocks.
  size_t bufferCount = elementCount * (2 * (iterations > 64?64:iterations) + 1);

  // Allocate memory for numbers
  memstart = malloc(sizeof(TEST_T) * (size_t)bufferCount);
  printf("[INFO] Allocated memory at 0x%4x\n", memstart);
  if(memstart == NULL) {
    printf("[ERR ] Memory allocation error! Exiting...\n");
        testResults_t err = {-1, -1};
    return err;
  }

  // out0 starts at the last dim*dim block
  out0 = memstart + (sizeof(TEST_T) * elementCount * 2 * (iterations > 64?64:iterations));

  // Populate
  startClock();
  srand(clock());
  GENERATEBLOCK_F(memstart, elementCount * 2 * (iterations>64?64:iterations));
  
  // Variables for testing
  float32_t execTimeSerial, execTimePIRF;
  
  // Serial Multiplication - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_T *temparray = (TEST_T*)malloc(dim * sizeof(TEST_T));
  size_t it;

  // Indexing blocks should be done as simply as possible...
  size_t blockSize = sizeof(TEST_T) * elementCount;
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    SERIALMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim, temparray);
  }

  clock_t endTime = readClock();
  execTimeSerial = clockToMS(endTime);

  printf("[DONE] Serial Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));


  // NEON Mult. (Input 2 Row First) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    IRFMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim);
  }
  endTime = readClock();
  execTimePIRF = clockToMS(endTime);

  printf("[DONE] P-IRF Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  free(memstart);
  free(temparray);

  testResults_t results;
  results.speedUp   = execTimeSerial / execTimePIRF;
  results.usPerIter = clockToMS((float32_t)endTime*100.0/(float32_t)iterations);

  return results;
}

#define TEST_T  float32_t
#define GENERATEBLOCK_F generateBlock32f
#define PRINTMATRIX_F   printMatrix32f
#define SERIALMULT_F    serialMatrixMult32f
#define IRFMULT_F       matrixMultIRF32f
testResults_t float32Test() {
  // Create elementCount*iterations pairs of input and space for one output.
  // More than 64 iterations will re-use blocks.
  size_t bufferCount = elementCount * (2 * (iterations > 64?64:iterations) + 1);

  // Allocate memory for numbers
  memstart = malloc(sizeof(TEST_T) * (size_t)bufferCount);
  printf("[INFO] Allocated memory at 0x%4x\n", memstart);
  if(memstart == NULL) {
    printf("[ERR ] Memory allocation error! Exiting...\n");
        testResults_t err = {-1, -1};
    return err;
  }

  // out0 starts at the last dim*dim block
  out0 = memstart + (sizeof(TEST_T) * elementCount * 2 * (iterations > 64?64:iterations));

  // Populate
  startClock();
  srand(clock());
  GENERATEBLOCK_F(memstart, elementCount * 2 * (iterations>64?64:iterations));

  // Variables for testing
  float32_t execTimeSerial, execTimePIRF;

  // Serial Multiplication - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_T *temparray = (TEST_T*)malloc(dim * sizeof(TEST_T));
  size_t it;

  // Indexing blocks should be done as simply as possible...
  size_t blockSize = sizeof(TEST_T) * elementCount;
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    SERIALMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim, temparray);
  }

  clock_t endTime = readClock();
  execTimeSerial = clockToMS(endTime);

  printf("[DONE] Serial Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));


  // NEON Mult. (Input 2 Row First) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    IRFMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim);
  }
  endTime = readClock();
  execTimePIRF = clockToMS(endTime);

  printf("[DONE] P-IRF Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  free(memstart);
  free(temparray);

  testResults_t results;
  results.speedUp   = execTimeSerial / execTimePIRF;
  results.usPerIter = clockToMS((float32_t)endTime*100.0/(float32_t)iterations);

  return results;
}

/* Not supported prior to ARMv8.2A
#define TEST_T  float16_t
#define GENERATEBLOCK_F generateBlock16f
#define PRINTMATRIX_F   printMatrix16f
#define SERIALMULT_F    serialMatrixMult16f
#define IRFMULT_F       matrixMultIRF16f
int float16Test() {
  // Create elementCount*iterations pairs of input and space for one output.
  // More than 64 iterations will re-use blocks.
  size_t bufferCount = elementCount * (2 * (iterations > 64?64:iterations) + 1);

  // Allocate memory for numbers
  memstart = malloc(sizeof(TEST_T) * (size_t)bufferCount);
  printf("[INFO] Allocated memory at 0x%4x\n", memstart);
  if(memstart == NULL) {
    printf("[ERR ] Memory allocation error! Exiting...\n");
    return 1;
  }

  // out0 starts at the last dim*dim block
  out0 = memstart + (sizeof(TEST_T) * elementCount * 2 * (iterations > 64?64:iterations));

  // Populate
  startClock();
  srand(clock());
  GENERATEBLOCK_F(memstart, elementCount * 2 * (iterations>64?64:iterations));

  // Serial Multiplication - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  TEST_T *temparray = (TEST_T*)malloc(dim * sizeof(TEST_T));
  size_t it;

  size_t blockSize = sizeof(TEST_T) * elementCount;
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    SERIALMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim, temparray);
  }

  clock_t endTime = readClock();

  printf("[DONE] Serial Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  // NEON Mult. (Input 2 Row First) - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Indexing blocks should be done as simply as possible...
  in1 = memstart;
  in2 = in1 + blockSize;

  startClock();
  for(it = 0; it < iterations; it++) {
    in1 = (it%64==0 ? memstart : in1+(blockSize<<1)); // Increase index by `blockSize` unless it should be reset
    in2 = in1 + blockSize;

    IRFMULT_F((TEST_T*)in1, (TEST_T*)in2, (TEST_T*)out0, dim);
  }
  endTime = readClock();

  printf("[DONE] P-IRF Matrix Multiplication (%2dx%2d):\t%4.2fms for %d iterations ", dim, dim, clockToMS(endTime), iterations);
  printf("(%4.2fus/it.)\n", clockToMS((float32_t)endTime*100.0/(float32_t)iterations));

  free(memstart);
  free(temparray);

  return 0;
}
*/

