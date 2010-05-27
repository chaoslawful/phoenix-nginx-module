#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

//#define DIRECT_SIGNAL

/* Generate SIGSEGV */
void gen_segv()
{
#ifdef DIRECT_SIGNAL
	raise(SIGSEGV);
#else
	int a=*(int*)0;
#endif
}

/* Generate SIGILL */
void gen_ill()
{
#ifdef DIRECT_SIGNAL
	raise(SIGILL);
#else
	char ins[]={0xff,0xff,0xff,0xff};
	((void(*)())ins)();
#endif
}

/* Generate SIGBUS */
void gen_bus()
{
#ifdef DIRECT_SIGNAL
	raise(SIGBUS);
#else
	FILE *fp=tmpfile();
	char *p=mmap(NULL,128,PROT_READ,MAP_SHARED,fileno(fp),0);
	int a=*p;
#endif
}

/* Generate SIGFPE */
void gen_fpe()
{
#ifdef DIRECT_SIGNAL
	raise(SIGFPE);
#else
	int a=0/0;
#endif
}

/* Generate SIGABRT */
void gen_abrt()
{
#ifdef DIRECT_SIGNAL
	raise(SIGABRT);
#else
	abort();
#endif
}

/* Generate heap corruption, to test whether fork() would be dead-locked */
void gen_heap_corrupt()
{
	/*
	 * fork() is not fully-compliant with POSIX.1-2003, which requires it to be
	 * always async-signal-safe. But in glibc implementations, fork() uses
	 * malloc() to allocate internal resources, and malloc() uses normal mutex
	 * to lock global memory pool. Normal mutex can not be nested, so if
	 * phoenix signal handler caught a exception raised by malloc(), it will be
	 * dead-locked when calling fork(). This violated async-signal-safe
	 * requirement of fork(), but has been listed as WONTFIX by kernel
	 * developers.
	 *
	 * Anyone who want to do fork/exec pair in signal handlers or multi-thread
	 * environment should turn to posix_spawn(), though it is not as powerful
	 * as fork/exec pair.
	 *
	 * Ref: http://sourceware.org/bugzilla/show_bug.cgi?id=4737
	 * */
	char *a = malloc(1);
	free(a);
	// corrupt internal malloc info
	memset(a-32, 0, 32);
	malloc(1);	// Oops...malloc() raise heap corruption exception
}

int main(int argc, char *argv[])
{
	typedef void (*crash_func)();
	static crash_func func_list[]= {
		gen_segv,
		gen_ill,
		gen_fpe,
		gen_abrt,
		gen_bus,
		gen_heap_corrupt
	};
	static int func_num = sizeof(func_list)/sizeof(func_list[0]);
	int crash_idx;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <crash number>\n", argv[0]);
		exit(-1);
	}

	crash_idx = atoi(argv[1]);
	if(crash_idx < 0 || crash_idx >= func_num) {
		fprintf(stderr, "Error: crash number out of range: must between 0~%d\n", func_num-1);
		exit(-1);
	}

	func_list[crash_idx]();
}

