#include "opencl101.h"
#define PRINT_OUT_RESULTS

cl_int runCL(const float *a, const float *b, float *results, size_t size)
{
	cl_program program;
	cl_kernel kernel;
	
	cl_int err = 0;  // OpenCL error
	cl_uint num_platforms;
	size_t returned_size = 0;  // clGetDeviceInfo
	size_t buffer_size;        // device buffer size

	cl_platform_id first_platform_id = NULL;
	cl_device_id cpu = NULL, device = NULL;

	cl_context context;
	cl_command_queue commands;

	size_t global_work_size = size;  /* problem size */
	/* size_t local_work_size = 256;    /\* group size *\/ */

	cl_mem a_mem, b_mem, ans_mem;

	// 1. Define the platform
	// ======================
	// Grab the first available platform
	err = clGetPlatformIDs(1, &first_platform_id, &num_platforms);
	assert(err == CL_SUCCESS);

	// Find the CPU CL device as a fallback
	err = clGetDeviceIDs(first_platform_id, CL_DEVICE_TYPE_CPU, 1, &cpu,
						 NULL);
	assert(err == CL_SUCCESS);

	// Find the GPU CL device, this is what we really want
	// If there is no GPU device is CL capable, fall back to CPU
	err = clGetDeviceIDs(first_platform_id, CL_DEVICE_TYPE_GPU, 1, &device,
						 NULL);
	if (err != CL_SUCCESS) device = cpu;
	assert(device);

	// Get some information about the returned device
	cl_char vendor_name[1024] = {0};
	cl_char device_name[1024] = {0};
	err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor_name),
						  vendor_name, &returned_size);
	err |= clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name),
						  device_name, &returned_size);
	assert(err == CL_SUCCESS);
	printf("Connecting to device(%s) of vendor(%s)...\n", device_name,
		   vendor_name);

	// Create a simple context with a single device
	context = clCreateContext(0, 1, &device, NULL, NULL, &err);
	assert(err == CL_SUCCESS);

	// and also a command queue for the context
	commands = clCreateCommandQueue(context, device, 0, &err);
	assert(err == CL_SUCCESS);

	// 2. Create and build the program.
	// ================================
	// Load kernel source
	const char *filename = "vector_addition_kernel.cl";
	char *kernelSource = read_file(filename);

	// Build the program object
	program = clCreateProgramWithSource(context, 1,
										(const char**)&kernelSource,
										NULL, &err);
	assert(err == CL_SUCCESS);

	// Compile the program to create a "dynamic library" from
	// which specific kernels can be pulled.
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	// Fetch and print error messages
	if (err != CL_SUCCESS) {
		size_t len;
		char buffer[2048];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
							  sizeof(buffer), buffer, &len);
		printf("clBuildProgram ERROR: %s\n", buffer);
		exit(0);
	}

	// 3. Memory Allocation
	// =======================
	// Allocate memory on the device to hold our data and store
	// the results into.
	buffer_size = sizeof(float) * size;

	// Input array a
	a_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL,
						   NULL);
	// Input array b
	b_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, buffer_size, NULL,
						   NULL);

	// results array
	ans_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, buffer_size, NULL,
						   NULL);

	// 4. Define the kernel.
	// ====================
	// Create kernel object from the kernel function "vector_add"
	kernel = clCreateKernel(program, "vector_add", &err);
	assert(err == CL_SUCCESS);

	// Attach arguments to the kernel function "vector_add" to memory
	// objects.
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_mem);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_mem);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &ans_mem);

	// 5. Submit commands
	// ==================
	// Write buffers from host into global memory
	err = clEnqueueWriteBuffer(commands, a_mem, CL_TRUE, 0, buffer_size,
							   (void *)a, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(commands, b_mem, CL_TRUE, 0, buffer_size,
							   (void *)b, 0, NULL, NULL);
	assert(err == CL_SUCCESS);

	// Enqueue the kernel for execution
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL,
								 &global_work_size, NULL,
								 0, NULL, NULL);
	assert(err == CL_SUCCESS);

	// Wait until all previous commands are done, the read back
	// the result
	clFinish(commands);
	err = clEnqueueReadBuffer(commands, ans_mem, CL_TRUE, 0, buffer_size,
							  results, 0, NULL, NULL);
	assert(err == CL_SUCCESS);
	clFinish(commands);

	// 6. Tear down.
	// =============
	clReleaseMemObject(a_mem);
	clReleaseMemObject(b_mem);
	clReleaseMemObject(ans_mem);

	clReleaseCommandQueue(commands);
	clReleaseContext(context);

	// Free the kernel source
	free(kernelSource);

	return CL_SUCCESS;
}

int main(int argc, const char *argv[])
{
	// Problem size
	size_t size = 1 << 20;

	// Allocate some memory and a place for the results
	float *a = (float *) malloc(sizeof(float) * size);
	float *b = (float *) malloc(sizeof(float) * size);
	float *results = (float *) malloc(sizeof(float) * size);

	// Fill in the values
	int i;
	for (i = 0; i < size; ++i) {
		a[i] = (float) i;
		b[i] = (float) (size - i);
		results[i] = 0.f;
	}

	// Dp the OpenCL calculation
	runCL(a, b, results, size);

	// Print out some results
#ifdef PRINT_OUT_RESULTS
	int j;
	for (j = 0; j < size; ++j) {
		printf("%f\n", results[j]);
	}
#endif
	
	printf("Done!");

	// Free up memories
	free(a);
	free(b);
	free(results);

	return 0;
}
