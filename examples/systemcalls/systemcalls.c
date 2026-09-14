#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

bool do_system(const char *cmd)
{
    if (cmd == NULL) return false;
    int status = system(cmd);
    if (status == -1) return false;
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count + 1];
    for(int i=0; i<count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    va_end(args);

    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) return false;
    if (pid == 0) {
        execv(command[0], command);
        perror("execv failed");
        exit(EXIT_FAILURE);
    }
    int status;
    if (waitpid(pid, &status, 0) == -1) return false;
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}

bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count + 1];
    for(int i=0; i<count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    va_end(args);

    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) return false;

    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) { close(fd); return false; }
    if (pid == 0) {
        if (dup2(fd, 1) < 0) {
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
        execv(command[0], command);
        exit(EXIT_FAILURE);
    }
    close(fd);
    int status;
    if (waitpid(pid, &status, 0) == -1) return false;
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}
