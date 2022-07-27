#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {

    FILE *fp;
    int count;
    char character;

    if (argc > 1) { /* 1 or more file(s) given */
        for (int i = 1; i < argc; ++i) {
            if ((fp = fopen(argv[i], "rb")) == NULL) {
                printf("wunzip: cannot open file\n");
                exit(EXIT_FAILURE);
            }
            while(fread(&count, 4, 1, fp)) {    /* Read int first */
                fread(&character, 1, 1, fp);    /* Read common char */
                for (int i = 0; i < count; ++i) putchar(character); /* Result in stdout */
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