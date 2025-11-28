#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    
}

int main (void){
    char *cmd[] = {"echo", "nigga",NULL};
    char *cmd2[] = {"cat", NULL};
    char **cmds[] = {cmd, cmd2, NULL};

    picoshell(cmds);
    write(1, "\n", 1);
}