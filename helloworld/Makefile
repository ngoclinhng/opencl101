INCLUDE_DIR = ../include
HEADER = ../include/opencl101.h
UTILS = ../common/utils.c

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3 -I$(INCLUDE_DIR)
OS := $(shell uname)
OPTIONS:=

ifeq ($(OS), Darwin)
	OPTIONS += -framework OpenCL
else
	OPTIONS += -l OpenCL
endif

all: main

main: main.c $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^ $(OPTIONS)

test: all
	time ./main

clean:
	$(RM) main *.o



