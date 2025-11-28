#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t pid;
    int status;
    
    pid = fork();
    
    if (pid == -1)
        return (-1);
    
    if (pid == 0)  // Child
    {
        signal(SIGALRM, SIG_DFL);
        alarm(timeout);
        f();
        exit(0);
    }
    
    // Parent
    if (waitpid(pid, &status, 0) == -1)
        return (-1);
    
    // Cancel any pending alarms (extra safety)
    alarm(0);
    
    // Killed by signal
    if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        
        if (sig == SIGALRM)
        {
            if (verbose)
                printf("Bad function: timed out after %u seconds\n", timeout);
            return (0);
        }
        
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig));
        return (0);
    }
    
    // Exited normally
    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        
        if (exit_code == 0)
        {
            if (verbose)
                printf("Nice function!\n");
            return (1);
        }
        
        if (verbose)
            printf("Bad function: exited with code %d\n", exit_code);
        return (0);
    }
    
    return (-1);
}