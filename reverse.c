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
            // Write standard input to temporary file.
            // Seeking back in reverse() is not possible if input is standard input.
            in = tmpfile();
            while ((c = fgetc(stdin)) != EOF) fputc(c, in);
            rewind(in); // Set input file position to beginning.
            break;
        case 3:
            // Check if input and output are the same file.
            stat(argv[1], &sin);
            stat(argv[2], &sout);
            if (sin.st_ino == sout.st_ino) {
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
            // Open output stream.
            out = fopen(argv[2], "w");
            if (out == NULL) {
                fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
                exit(1);
            }
            // No break, continue to case 2.
        case 2:
            // Open input stream.
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
    fclose(in);                 // Close input stream.
    if (argc == 3) fclose(out); // Close output stream.
    return 0;
}


void reverse(FILE *in, FILE *out) {

    long start = ftell(in);     // File start position.
    fseek(in, -1L, SEEK_END);   // Seek to the end of the file, skip the last line break.
    long end = ftell(in);       // Position of last character on the current line.
    long here;                  // Current position.

    while (fseek(in, -2L, SEEK_CUR) != -1) {    // Go back in file until back to start.
        if (fgetc(in) == '\n') {                // If line break has been found
            here = ftell(in);                   // Update current position.
            for (long i = here; i <= end; ++i) {
                fprintf(out, "%c", fgetc(in));  // Write previous line.
            }
            fseek(in, here - end - 1, SEEK_CUR);// Seek back to current position.
            end = here - 1;                     // Update last character position.
        }
    }                                       // All but the first line has been printed.
    fseek(in, -1L, SEEK_CUR);               // Seek back to start.
    for (long i = start; i <= end; ++i) {
        fprintf(out, "%c", fgetc(in));      // Write the first line.
    }
}
