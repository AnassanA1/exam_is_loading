#include <unistd.h>
#include <stdlib.h>

int     ft_popen(const char *file, char *const argv[], char type)
{
    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    
    pid_t pid;
    int fd[2];
    int status;

    if (pipe(fd) == -1)
        return (-1);

    pid = fork();
    if (pid < 0)
        return (-1);

    else if (pid == 0)
    {
        if (type == 'r')
        {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            close(fd[0]);
        }
        else if (type == 'w')
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
        }
        execvp(file, argv);
        _exit(1);
    }

    if (waitpid(pid, &status, 1) == -1)
        return (-1);

    if (type == 'r')
    {
        close(fd[1]);
        return (fd[0]);
    }
    else if (type == 'w')
    {
        close(fd[0]);
        return(fd[1]);
    }

    return (-1);
}

int main() {
        int fd = ft_popen("wc", (char *const[]){"wc", "-l", NULL}, 'w');

        char buff[1200];
        int byte_read = 0;
        while ((byte_read = read(fd, buff, 1199)) > 0)
        {
            buff[byte_read] = '\0';
            write(1, buff, byte_read);
        }
}