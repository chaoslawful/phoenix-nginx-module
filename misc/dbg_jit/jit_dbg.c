#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void sighandler(int signo)
{
	char buf[255];
	int status;
	pid_t org_pid=getpid();
	pid_t pid;
	snprintf(buf,sizeof(buf),"attach %d",org_pid);
	pid=fork();
	if(pid==0) {
		if(execl("/usr/bin/gdb","gdb","-ex",buf,NULL)==-1) {
			perror("Error: could not execute gdb");
		}
		goto out;
	}
	if(wait(&status)<0) {
		perror("wait on gdb");
		goto out;
	}
out:
	raise(signo);	
}

__attribute__((constructor)) void ctor()
{
	struct sigaction sa;
	sa.sa_handler=sighandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESETHAND;
	if(sigaction(SIGILL,&sa,NULL)==-1 ||
		sigaction(SIGFPE,&sa,NULL)==-1 ||
		sigaction(SIGABRT,&sa,NULL)==-1 ||
		sigaction(SIGBUS,&sa,NULL)==-1 ||
		sigaction(SIGSEGV,&sa,NULL)==-1) {
		perror("Could not set signal handler");
	}
}

