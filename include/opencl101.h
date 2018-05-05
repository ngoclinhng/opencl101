#ifndef OPENCL_101_H
#define OPENCL_101_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

char *read_file(const char *filename);

#endif
