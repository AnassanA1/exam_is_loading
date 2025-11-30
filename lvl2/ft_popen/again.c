#include <unistd.h>
#include <stdlib.h>

int     ft_popen(const char *file, char *const argv[], char type)
{
    if (!file || !argv || (type != 'w' && type != 'r'))
        return (-1);

    pid_t pid;
    int fd[2];

    if (pipe(fd) < 0)
        return (-1);
    
    pid = fork();
    if (pid < 0)
        return (-1);
    
    if (pid == 0)
    {
        if (type == 'w')
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
        }
        else if (type == 'r')
        {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            close(fd[0]);
        }
        execvp(file, argv);
        _exit(1);
    }

    if (type == 'w')
    {
        close(fd[0]);
        return (fd[1]);
    }
    else if (type == 'r')
    {
        close(fd[1]);
        return (fd[0]);
    }

    return (-1);
}

int main() {
	int fd = ft_popen("wc", (char *const[]){"wc", "-l" , NULL}, 'w');

    char buf[1201];
    int ret = 0;

    while ((ret = read(fd, buf, 1200)) > 0)
    {
        buf[ret] = '\0';
        write(1, buf, ret);
    }
    close(fd);
    return (0);
}
