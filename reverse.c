#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


void reverse(FILE*, FILE*);


int main(int argc, char *argv[]) {
    
    FILE *in, *out = stdout;
    char c;
    struct stat sin, sout;

    switch (argc) {
        case 1:
            in = tmpfile();
            while ((c = fgetc(stdin)) != EOF) fputc(c, in);
            rewind(in);
            break;
        case 3:
            stat(argv[1], &sin);
            stat(argv[2], &sout);
            if (sin.st_ino == sout.st_ino) {
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
            out = fopen(argv[2], "w");
            if (out == NULL) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
                exit(1);
            }
        case 2:
            in = fopen(argv[1], "r");
            if (in == NULL) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "usage: reverse <input> <output>\n");
            exit(1);
            break;
    }
    reverse(in, out);
    if (argc > 1) fclose(in);
    if (argc == 3) fclose(out);
    return 0;
}


void reverse(FILE *in, FILE *out) {

    long start = ftell(in);
    fseek(in, -1L, SEEK_END);
    long end = ftell(in);
    long here;

    while (fseek(in, -2L, SEEK_CUR) != -1) {
        if (fgetc(in) == '\n') {
            here = ftell(in);
            for (long i = here; i <= end; ++i) fprintf(out, "%c", fgetc(in));
            fseek(in, here - end - 1, SEEK_CUR);
            end = here - 1;
        }
    }
    fseek(in, -1L, SEEK_CUR);
    for (long i = start; i <= end; ++i) fprintf(out, "%c", fgetc(in));
}
