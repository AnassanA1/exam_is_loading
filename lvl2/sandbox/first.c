#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    int status;
    pid_t pid;

    pid = fork();
    if (pid < 0)
        return (-1);
    if (pid == 0)
    {
        alarm(timeout);
        f();
        exit(0);
    }

    if (waitpid(pid, &status, 0) == -1)
        return (-1);
    
    alarm(0);

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

void infinite_loop(void) {
}

int main(void) {
    printf("Result: %d\n", sandbox(infinite_loop, 2, true));
    return 0;
}