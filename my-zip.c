/*
my-zip: compressing file by writing consecutive characters to one integer
and one character. Supports reading multiple files.
*/

#define _GNU_SOURCE /* For getline */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    size_t size;
    size_t llen;
    char *line;
    FILE *fp;

    char character = 0;
    int count = 0;

    if (argc > 1) { /* 1 or more files given */
        for (int i = 1; i < argc; ++i) {
            if ((fp = fopen(argv[i], "r")) == NULL) {
                printf("wzip: cannot open file\n");
                exit(EXIT_FAILURE);
            }
            while((llen = getline(&line, &size, fp)) != -1) {
                character = character ? character: line[0]; /* Check if characer is defined */
                for (size_t j = 0; j < llen; ++j) { /* Read line character by character */
                    if (line[j] != character) { /* Character changes */
                        fwrite(&count, 4, 1, stdout);
                        fwrite(&character, 1, 1, stdout);
                        character = line[j];
                        count = 0;
                    }
                    ++count;
                }
            }
            free(line);
            fclose(fp);
        }
        /* Writing last character which is not written inside the loop */
        fwrite(&count, 4, 1, stdout);
        fwrite(&character, 1, 1, stdout);
    }
    else {
        printf("wzip: file1 [file2 ...]\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
