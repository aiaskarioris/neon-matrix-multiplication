CC = aarch64-linux-gnu-gcc 
GCC-FLAGS = -march=armv8-a -mtune=cortex-a53 -O2 -ftree-vectorize
TARGET = mmtest
OUTPUTDIR = build

SOURCE=${wildcard src/*.c}
OBJS := ${SOURCE:.c=.o}

all: compile clean

.c.o:
	$(CC) $(GCC-FLAGS-SIMD) -c -g -o $@ $<  
compile: $(OBJS)
	$(CC) $(GCC-FLAGS-SIMD)    -o $(OUTPUTDIR)/$(TARGET) $(OBJS)
	$(CC) $(GCC-FLAGS-SIMD) -g -o $(OUTPUTDIR)/$(TARGET)-debug $(OBJS)
clean:
	rm -r src/*.o
