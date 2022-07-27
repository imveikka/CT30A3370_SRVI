
#define _GNU_SOURCE /* For getline */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


char* scan(FILE*);
char** execinput(char*, char**);


int main(int argc, char *argv[]) {
    char errmsg[] = "An error has occurred\n";
    char **path = (char *[]) {"/bin/", NULL};
    char *input;
    FILE* fp;
    switch (argc) {
        case 1:
            for (;;) {
                printf("wish> ");
                input = scan(stdin);
                path = execinput(input, path);
            }
            break;
        case 2:
            if (!(fp = fopen(argv[1], "r"))) {
                fprintf(stderr, "%s", errmsg);
                exit(1);
            }
            while ((input = scan(fp))) path = execinput(input, path);
            fclose(fp);
            break;
        default:
            fprintf(stderr, "%s", errmsg);
            exit(1);
    }
    return 0;
}


char* scan(FILE *fp) {
    char *input;
    size_t bufsize = 0, len;
    len = getline(&input, &bufsize, fp);
    if (len == -1) return NULL;
    if (input[len - 1] == '\n') input[len - 1] = '\0';
    return input;
}


char** execinput(char* input, char** path) {
    char errmsg[] = "An error has occurred\n";
    char delimit[] = " \t\r\n\v\f";
    char *pararrel, *command, *dir, *token;
    char *saveptr1, *saveptr2, *saveptr3;
    char **args = NULL;
    int c = 0, npid = 0;
    pararrel = strtok_r(input, "&", &saveptr1);
    while (pararrel) {
        if (pararrel[strlen(pararrel) - 1] == '>') {
            fprintf(stderr, "%s", errmsg);
            break;
        }
        command = strtok_r(pararrel, ">", &saveptr2);
        dir = strtok_r(NULL, delimit, &saveptr2);
        if (strtok_r(NULL, "> \t\r\n\v\f", &saveptr2)) {
            fprintf(stderr, "%s", errmsg);
            break;
        }
        token = strtok_r(command, delimit, &saveptr3);
        if (!token) break;
        while (token) {
            args = realloc(args, sizeof(char*) * (c + 1));
            char *arg = strdup(token);
            args[c++] = arg;
            token = strtok_r(NULL, delimit, &saveptr3);
        }
        args = realloc(args, sizeof(char*) * (c + 1));
        args[c] = NULL;        
        if (!strcmp(args[0], "exit")) {
            if (c == 1) exit(0);
            else fprintf(stderr, "%s", errmsg);
        } else if (!strcmp(args[0], "cd")) {
            if (c != 2 || chdir(args[1])) fprintf(stderr, "%s", errmsg);
        } else if (!strcmp(args[0], "path")) {
            int i = 1;
            path = NULL;
            while (args[i]) {
                char *rpath;
                if ((rpath = realpath(args[i], NULL))) {
                    strcat(rpath, "/");
                    path = realloc(path, sizeof(char*) * i);
                    path[i - 1] = rpath;
                    i++;
                }
            }
            path = realloc(path, sizeof(char*) * i);
            path[i - 1] = NULL;
        } else {
            int i = 0;
            int found = 0;
            while (path[i]) {
                char *fullpath = strdup(path[i++]);
                strcat(fullpath, args[0]);
                if (!access(fullpath, X_OK)) {
                    int pid;
                    if ((pid = fork()) == -1) fprintf(stderr, "failed to fork\n");
                    else if (pid == 0) {
                        if (dir) {
                            int fd = open(dir, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                            dup2(fd, STDOUT_FILENO);
                            dup2(fd, STDERR_FILENO);
                            close(fd);
                        }
                        execv(fullpath, args);
                    } else npid++;
                    found = 1;
                    break;
                }
            }
            if (!found) fprintf(stderr, "%s", errmsg);
        }
        args = NULL;
        c = 0;
        pararrel = strtok_r(NULL, "&", &saveptr1);
    }
    while (npid) {
        wait(NULL);
        npid--;
    }
    return path;
}
