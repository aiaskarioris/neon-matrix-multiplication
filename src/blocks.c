#include "header.h"

// === Block Generation ===========================================================================
void generateBlock8(int8_t *buffer, size_t count) {
  size_t i;
  // Populate buffers
  int8_t randArray[32];
  for(i = 0; i < 32; i++) { randArray[i] = rand(); }
  int8_t mask = rand();

  printf("[  0%%] Populating buffer(s)...");
  for(i = 0; i < count; i++) {
    if(i%128 == 0) { printf("\r[%3d%%] Populating buffer(s)...", 100*i/count);  }

    buffer[i]  = randArray[i%32] ^ i;
    mask = (mask << 1) | ((mask & 0x80) >> 7);
    buffer[i]  = buffer[i] % 17;
    buffer[i] *= (mask % 2) ? -1 : 1;
  }

  printf("\r[100%%] Populating buffer(s)...\n");
}

void generateBlock16(int16_t *buffer, size_t count) {
  size_t i;
  // Populate buffers
  int16_t randArray[32];
  for(i = 0; i < 32; i++) { randArray[i] = rand(); }
  int8_t mask = rand();

  printf("[  0%%] Populating buffer(s)...");
  for(i = 0; i < count; i++) {
    if(i%128 == 0) { printf("\r[%3d%%] Populating buffer(s)...", 100*i/count);  }
    buffer[i]  = randArray[i%32] ^ i;
    mask = (mask << 1) | ((mask & 0x8000) >> 15);
    buffer[i]  = buffer[i] % 19;
    buffer[i] *= (mask % 2) ? -1 : 1;
  }

  printf("\r[100%%] Populating buffer(s)...\n");
}


void generateBlock32(int32_t *buffer, size_t count) {
  size_t i;
  // Populate buffers
  int32_t randArray[64];
  for(i = 0; i < 16; i++) { randArray[i] = rand(); }
  int32_t mask = rand();

  printf("[  0%%] Populating buffer(s)...");
  for(i = 0; i < count; i++) {
    if(i%128 == 0) { printf("\r[%3d%%] Populating buffer(s)...", 100*i/count);  }
    buffer[i]  = randArray[i%64] ^ i;
    mask = (mask << 1) | ((mask & 0x80000000) >> 31);
    buffer[i]  = buffer[i] % 15;
    buffer[i] *= (mask % 2) ? -1 : 1;
  }

  printf("\r[100%%] Populating buffer(s)...\n");
}

void generateBlock32f(float32_t *buffer, size_t count) {
  size_t i;
  // Populate buffers
  float32_t randArray[32];
  for(i = 0; i < 32; i++) {
    int32_t temp = rand() & 0x7FFF;
    randArray[i] = 1.0/(float32_t)temp;
  }

  for(i = 0; i < count; i++) {
    if(i%128 == 0) { printf("\r[%3d%%] Populating buffer(s)...", 100*i/count);  }

    buffer[i]  = randArray[i%32];
  }

  printf("\r[100%%] Populating buffer(s)...\n");
}


