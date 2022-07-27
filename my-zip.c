
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

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if ((fp = fopen(argv[i], "r")) == NULL) {
                printf("wzip: cannot open file\n");
                exit(EXIT_FAILURE);
            }
            while((llen = getline(&line, &size, fp)) != -1) {
                character = character ? character: line[0];
                for (size_t j = 0; j < llen; ++j) {
                    if (line[j] != character) {
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
        fwrite(&count, 4, 1, stdout);
        fwrite(&character, 1, 1, stdout);
    }
    else {
        printf("wzip: file1 [file2 ...]\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
