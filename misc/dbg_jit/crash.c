#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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

int main()
{
	gen_segv();
//	gen_ill();
//	gen_fpe();
//	gen_abrt();
//	gen_bus();
}

