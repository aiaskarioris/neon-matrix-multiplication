#include "header.h"

void printMatrix8(int8_t *buffer, const char *buffername, size_t dim) {
    printf("\n%s (@%6x) ", buffername, buffer);
    for(size_t i = 0; i < dim*dim; i++) {
        if(i % dim == 0) { printf(";\n"); }
        printf("  %3d", buffer[i]);
    }
    printf("\n");
}

void printMatrix16(int16_t *buffer, const char *buffername, size_t dim) {
    printf("\n%s (@%6x) ", buffername, buffer);
    for(size_t i = 0; i < dim*dim; i++) {
        if(i % dim == 0) { printf(";\n"); }
        printf("  %4d", buffer[i]);
    }
    printf("\n");
}

void printMatrix32(int32_t *buffer, const char *buffername, size_t dim) {
    printf("\n%s (@%6x) ", buffername, buffer);
    for(size_t i = 0; i < dim*dim; i++) {
        if(i % dim == 0) { printf(";\n"); }
        printf("  %4d", buffer[i]);
    }
    printf("\n");
}

void printMatrix32f(float32_t *buffer, const char *buffername, size_t dim) {
    printf("\n%s (@%6x) ", buffername, buffer);
    for(size_t i = 0; i < dim*dim; i++) {
        if(i % dim == 0) { printf(";\n"); }
        printf("  %2.4f", buffer[i]);
    }
    printf("\n");
}



