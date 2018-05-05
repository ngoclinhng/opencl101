/**
 * utils.c - A collection of utilities used in OpenCL projects
 */
#include "opencl101.h"

/****************************************************
 * WRAPPERS for system calls
 ****************************************************/
/**
 * unix_error() reports errno to stderr and terminates the process.
 */
static void unix_error(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

/**
 * Fopen() is the wrapper for fopen()
 */
static FILE *Fopen(const char *filename, const char *mode)
{
	FILE *fp;

	if ((fp = fopen(filename, mode)) == NULL)
		unix_error("Fopen error");
	return fp;
}

/**
 * Fread() is the wrapper for fread().
 */
static size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t nr;

	if (((nr = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream))
		unix_error("Fread error");
	return nr;
}

/**
 * Fclose() is the wrapper for fclose().
 */
static void Fclose(FILE *fp)
{
	if (fclose(fp) != 0)
		unix_error("Fclose error");
}

/**
 * Stat() is the wrapper for stat().
 */
void Stat(const char *filename, struct stat *buf)
{
	if (stat(filename, buf) < 0)
		unix_error("Stat error");
}

/*****************************************************
 * UTILS FOR OpenCL programs
 ****************************************************/
#define MAXBUF 8192  /* Max I/O buffer size */

/**
 * read_file() reads the content of the file whose name is pointed to
 * by `filename` into a dynamicly allocated buffer.
 * On success: return the destination buffer pointer to null terminated string
 * On failure: prints errno to stderr and exit.
 * The caller is resposible for deallocating memory!
 */
char *read_file(const char *filename)
{
	struct stat statbuf;
	FILE *fp;
	char *dst;

	fp = Fopen(filename, "r");
	Stat(filename, &statbuf);
	dst = (char *) malloc(statbuf.st_size + 1);
	
	Fread(dst, statbuf.st_size, 1, fp);
	dst[statbuf.st_size] = '\0';
	Fclose(fp);
	return dst;
}
