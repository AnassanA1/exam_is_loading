#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{   
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0)
        return (-1);
    else if (pid == 0)
    {
        alarm(timeout);
        f();
        _exit(0);
    }

    alarm(0);

    if (waitpid(pid, &status, 0) < 0)
        return (-1);
    
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

    if (WIFEXITED(status))
    {
        int exit_code =  WEXITSTATUS(status);
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

void do_abort()
{
    abort();
}

int main()
{
    printf("%d\n", sandbox(do_abort, 3, true));
}