INCLUDE_DIR = ../include
HEADER = ../include/opencl101.h
UTILS = ../common/utils.c

OS := $(shell uname -s)

ifeq ($(OS), Darwin)
    CC := clang
    CFLAGS := -Os -Wall -arch x86_64 -I$(INCLUDE_DIR)
    OPTIONS := -framework OpenCL
else
    CC := gcc
    CFLAGS := -std=c99 -Wall -pedantic -g3 -I$(INCLUDE_DIR)
    OPTIONS := -l OpenCL
endif

all: main

main: main.c $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^ $(OPTIONS)

test: all
	time ./main

clean:
	$(RM) main *.o



