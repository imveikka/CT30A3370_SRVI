#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    // size_t size;
    // size_t llen;
    // char *line;
    FILE *fp;
    int count;
    char character;
    // char character = 0;
    // int count = 0;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if ((fp = fopen(argv[i], "rb")) == NULL) {
                printf("wunzip: cannot open file\n");
                exit(EXIT_FAILURE);
            }
            while(fread(&count, 4, 1, fp)) {
                fread(&character, 1, 1, fp);
                for (int i = 0; i < count; ++i) putchar(character);
            }
        }
        fclose(fp);
    }
    else {
        printf("wunzip: file1 [file2 ...]\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}