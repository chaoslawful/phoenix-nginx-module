#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>

#ifndef DEFAULT_GDB_PATH
#define DEFAULT_GDB_PATH "/usr/bin/gdb"
#endif
#ifndef DEFAULT_OUT_FILE
#define DEFAULT_OUT_FILE "./jit_dbg.log"
#endif

#define ENV_GDB_PATH "JITDBG_GDB_PATH"
#define ENV_OUT_FILE "JITDBG_OUT_FILE"

void sighandler(int signo)
{
    char *val;
    char gdb_path[255];
    char set_logging_file[255];
    char buf[255];
    int status;
    pid_t pid;

    snprintf(gdb_path, sizeof(gdb_path),
            "%s",
            (val = getenv(ENV_GDB_PATH)) ? val : DEFAULT_GDB_PATH);
    snprintf(set_logging_file, sizeof(set_logging_file),
            "set logging file %s",
            (val = getenv(ENV_OUT_FILE)) ? val : DEFAULT_OUT_FILE);
    snprintf(buf, sizeof(buf), "%d", getpid());

    // spawn a individual gdb process and dumping backtrace of this process
    {
        char* argv[] = {
            gdb_path,
            "--batch",
            "-ex", "set width 0",
            "-ex", "set pagination off",
            "-ex", set_logging_file,
            "-ex", "set logging redirect on",
            "-ex", "set logging on",
            "-ex", "info proc",
            "-ex", "bt f",
            "--pid", buf
        };
        char* envp[] = {
            NULL
        };

        if (posix_spawn(&pid, gdb_path, NULL, NULL, argv, envp) < 0) {
            perror("Error: could not spawn gdb");
            goto out;
        }
    }

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid on gdb");
        goto out;
    }

out:
    // raise the signal again to crash process
    raise(signo);
}

__attribute__ ((constructor))
void ctor()
{
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;
    if (sigaction(SIGILL, &sa, NULL) == -1 ||
        sigaction(SIGFPE, &sa, NULL) == -1 ||
        sigaction(SIGABRT, &sa, NULL) == -1 ||
        sigaction(SIGBUS, &sa, NULL) == -1 ||
        sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Could not set signal handler");
    }
}

// vi:ts=4 sw=4 et fdm=marker

