/*
wgrep: a minimalist grep tool to find user-specified search term from given files
or stdin. Prints each line in stdout that has given search term.
Error messages are printed in stdout as well due to design requirements
*/

#define _GNU_SOURCE /* For getline */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void grep(char *, size_t, FILE *);
int match(char *, size_t, char *, size_t);


int main(int argc, char *argv[]) {
    FILE *in;
    char c;
    size_t p_len;
    /* Choose correct function(s) */
    switch (argc) {
        case 1:     // No arguments
            printf("wgrep: searchterm [file ...]\n");
            exit(1);
            break;
        case 2:     // Text given from stdin
            p_len = strlen(argv[1]);
            if ((in = tmpfile()) == NULL) exit(1);
            while ((c = fgetc(stdin)) != EOF) fputc(c, in);
            rewind(in);
            grep(argv[1], p_len, in);
            fclose(in);
            break;
        default:    // Text given from 1 or more files
            p_len = strlen(argv[1]);
            for (int i = 2; i < argc; ++i) {
                if ((in = fopen(argv[i], "r")) == NULL) {
                    printf("wgrep: cannot open file\n");
                    exit(1);
                }
                grep(argv[1], p_len, in);
                fclose(in);
            }
            break;
    }
}


/* Reads each line from a file and print lines with search term */
void grep(char *pattern, size_t p_len, FILE *in) {
    char *line = NULL;
    size_t s_size = 0;
    size_t s_len;
    while ((s_len = getline(&line, &s_size, in)) != -1)
        if (match(line, s_len, pattern, p_len)) printf("%s", line);
    free(line);
}


/* Checks if a given pattern is fount in string */
int match(char *string, size_t s_len, char *pattern, size_t p_len) {
    size_t m_len;
    if (p_len < s_len) {
        for (size_t i = 0; i <= s_len - p_len; ++i) {
            for (m_len = 0; m_len < p_len; ++m_len) if (pattern[m_len] != string[i+m_len]) break;
            if (m_len == p_len) return 1;
        }
    }
    return 0;
}
