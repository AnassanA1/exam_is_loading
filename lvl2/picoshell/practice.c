#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
    pid_t pid;
    pid_t child_processes[128];
    int i = 0;
    int prev_fd = -1;
    int child_count = 0;
    int pipe_fd[2];

    while (cmds[i])
    {
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
        else if (pid == 0)
        {
            if (cmds[i + 1])
            {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
                close(pipe_fd[0]);
            }
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            execvp(cmds[i][0], cmds[i]);
            _exit(1);
        }
        child_processes[child_count++] = pid;

        if (prev_fd != -1)
            close(prev_fd);

        if (cmds[i + 1])
        {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }
        
        i++;
    }

    int status;
    int res = 0;
    for (int j = 0; j < child_count; j++)
    {
        if (waitpid(child_processes[j], &status, 0) < 0)
            return (1);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            res = 1;
    }
    return (res);
}

int main(void)
{
    char *cmd[] = {"ls", "-l", NULL};
    char *cmd2[] = {"grep", "practice", NULL};
    char **cmds[] = {cmd, cmd2, NULL};

    picoshell(cmds);
}