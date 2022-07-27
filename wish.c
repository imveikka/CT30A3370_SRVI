
#define _DEFAULT_SOURCE || _POSIX_C_SOURCE >= 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


char* scan(FILE*);
char** execinput(char*, char**);


int main(int argc, char *argv[]) {

    char **path = (char *[]) {"/bin/", NULL};   // Default path.
    char *input;
    FILE* fp;

    switch (argc) {
        case 1:
            // Interactive mode.
            for (;;) {
                printf("wish> ");
                input = scan(stdin);
                path = execinput(input, path);
            }
            break;
        case 2:
            // Batch file.
            if (!(fp = fopen(argv[1], "r"))) {
                fprintf(stderr, "wish: failed to open file '%s'\n", argv[1]);
                exit(1);
            }
            while ((input = scan(fp))) path = execinput(input, path);
            fclose(fp);
            break;
        default:
            // Too many arguments.
            fprintf(stderr, "wish: too many arguments\n");
            exit(1);
    }
    return 0;
}


/* Read input line from file stream */
char* scan(FILE *fp) {
    char *input;
    size_t bufsize = 0, len;
    len = getline(&input, &bufsize, fp);
    if (len == -1) return NULL;                         // Return null if there is no input.
    if (input[len - 1] == '\n') input[len - 1] = '\0';  // Remove line break.
    return input;
}


/* Execute input, returns updated path */ 
char** execinput(char* input, char** path) {

    char delimit[] = " \t\r\n\v\f";             // Whitespace characters.
    char *pararrel, *command, *dir, *token;
    char *saveptr1, *saveptr2, *saveptr3;       // Used internally by strtok_r().
    char **args = NULL;                         // Contains command and its arguments.
    int c = 0, npid = 0;                        // Number of child processes.

    // Pararrel strtok_r splits input to pararrel commands with '&' delimiter.
    pararrel = strtok_r(input, "&", &saveptr1);
    while (pararrel) {

        // Check if command ends with '>', redirection error.
        if (pararrel[strlen(pararrel) - 1] == '>') {
            fprintf(stderr, "wish: redirection error\n");
            break;
        }
        command = strtok_r(pararrel, ">", &saveptr2);   // Command.
        dir = strtok_r(NULL, delimit, &saveptr2);       // Redirect path.
        // Check if command has '>' or whitespace after redirect path, redirection error.
        if (strtok_r(NULL, "> \t\r\n\v\f", &saveptr2)) {
            fprintf(stderr, "wish: redirection error\n");
            break;
        }

        // Token strtok_r splits command to words with whitespace delimiters.
        token = strtok_r(command, delimit, &saveptr3);
        // Check if command is empty.
        if (!token) {
            fprintf(stderr, "wish: command not found\n");
            break;
        }
        while (token) {
            // Dynamically allocate memory for unspecified amount of command arguments.
            args = realloc(args, sizeof(char*) * (c + 1));
            char *arg = strdup(token);
            args[c++] = arg;
            token = strtok_r(NULL, delimit, &saveptr3);
        }
        args = realloc(args, sizeof(char*) * (c + 1));
        args[c] = NULL; // Last argument must be null for execv(). 

        // Built-in commands
        if (!strcmp(args[0], "exit")) {         // Built-in exit.
            if (c == 1) exit(0);
            else fprintf(stderr, "wish: exit takes no arguments\n");
        } else if (!strcmp(args[0], "cd")) {    // Built-in cd.
            if (c != 2 || chdir(args[1])) fprintf(stderr, "wish: cd no arguments or too many arguments\n");
        } else if (!strcmp(args[0], "path")) {  // Built-in path.
            int i = 0, j = 0;
            path = NULL;
            
            while (args[i++]) {
                char *rpath;
                // Check if path exists and find canonicalized absolute pathname.
                if ((rpath = realpath(args[i], NULL))) {
                    strcat(rpath, "/");                         // Add / to end of absolute path.
                    path = realloc(path, sizeof(char*) * i);    // Dynamically allocates memory for paths.
                    path[j++] = rpath;
                }
                i++;
            }
            path = realloc(path, sizeof(char*) * (j + 1));
            path[j] = NULL; // Path array must end with null.

        } else {

            int i = 0;
            int found = 0;  // Boolean, false in end if command has not been found.
            char *fullpath;

            // Try to find command in path array.
            while (path[i]) {
                fullpath = strdup(path[i++]);
                strcat(fullpath, args[0]);
                // Check if command is found in the path.
                if (!access(fullpath, X_OK)) {
                    // Create child process. 
                    int pid;
                    if ((pid = fork()) == -1) fprintf(stderr, "wish: failed to fork child process\n");
                    else if (pid == 0) {
                        if (dir) { // If redirection path is defined.
                            // Duplicate standard output and error file descriptions to redirection file.
                            int fd = open(dir, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                            dup2(fd, STDOUT_FILENO);
                            dup2(fd, STDERR_FILENO);
                            close(fd);
                        }
                        // Execute commands with arguments.
                        execv(fullpath, args);
                    } else npid++; // Called when child process is ready.
                    found = 1;
                    break;
                }
                free(fullpath);
            }
            if (!found) fprintf(stderr, "wish: command not found\n");
        }
        args = NULL;
        c = 0;

        pararrel = strtok_r(NULL, "&", &saveptr1);
    }

    // All pararrel commands have been started. Wait for each to return.
    while (npid) {
        wait(NULL);
        npid--;
    }

    return path;
}
