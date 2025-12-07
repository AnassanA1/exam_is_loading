#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int     sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0)
        return (-1);
    if (pid == 0)
    {
        alarm(timeout);
        f();
        _exit(0);
    }

    if (waitpid(pid, &status, 0) == -1)
        return (-1);

    
    
    if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (sig == SIGALRM)
        {
            if (verbose)
                printf("Bad function: timed out after %u seconds\n", timeout);
            kill(pid, SIGKILL);
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

void test_segfault(void) {
    strlen(NULL);
}

void test_abort(void) {
    abort();
}

void test_timeout(void) {
    while(1);
}

void test_exit_bad(void) {
    exit(42);
}

void test_nice(void) {
    return ;
}

// void test_div_zero(void) {
//     int x = 5 / 0;
//     (void)x;
// }

int main(void) {
    printf("Test 3: "); sandbox(test_abort, 5, true);
    printf("Test 1: "); sandbox(test_nice, 5, true);
    printf("Test 2: "); sandbox(test_segfault, 5, true);
    printf("Test 4: "); sandbox(test_timeout, 10, true);
    printf("Test 5: "); sandbox(test_exit_bad, 5, true);
    return 0;
}