#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int picoshell(char **cmds[])
{
    pid_t pid;
    pid_t child_process[128];
    int child_count = 0;
    int i = 0;
    int pipe_fd[2];
    int prev_fd = -1;

    while (cmds[i])
    {
        // Create pipe if not last command
        if (cmds[i + 1] && pipe(pipe_fd) == -1)
        {
            if (prev_fd != -1)
                close(prev_fd);
            return (1);
        }

        pid = fork();
        if (pid < 0)
        {
            if (cmds[i + 1])
            {
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }
            if (prev_fd != -1)
                close(prev_fd);
            return (1);
        }

        if (pid == 0)  // Child process
        {
            // Redirect stdin from previous pipe
            if (prev_fd != -1)  // FIXED: was == -1
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            // Redirect stdout to current pipe
            if (cmds[i + 1])
            {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                close(pipe_fd[0]);
            }

            execvp(cmds[i][0], cmds[i]);
            _exit(1);
        }

        // Parent process
        child_process[child_count++] = pid;

        // Close previous pipe read end
        if (prev_fd != -1)
            close(prev_fd);

        // Close current pipe write end and save read end
        if (cmds[i + 1])
        {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }

        i++;
    }

    // Wait for all children
    int status;
    int result = 0;
    for (int j = 0; j < child_count; j++)
    {
        if (waitpid(child_process[j], &status, 0) == -1)
            return (1);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            result = 1;
    }

    return (result);
}
int main(void)
{
    // Test 1: ls | grep main
    char *cmd1[] = {"ls", NULL};
    char *cmd2[] = {"grep", "main", NULL};
    char **cmds1[] = {cmd1, cmd2, NULL};
    
    printf("Test 1: ls | grep main\n");
    picoshell(cmds1);
    printf("\n");

    // Test 2: echo hello | cat
    char *cmd3[] = {"echo", "hello", NULL};
    char *cmd4[] = {"cat", NULL};
    char **cmds2[] = {cmd3, cmd4, NULL};
    
    printf("Test 2: echo hello | cat\n");
    picoshell(cmds2);
    printf("\n");

    // Test 3: Single command (no pipe)
    char *cmd5[] = {"echo", "single command", NULL};
    char **cmds3[] = {cmd5, NULL};
    
    printf("Test 3: echo single command\n");
    picoshell(cmds3);

    return 0;
}