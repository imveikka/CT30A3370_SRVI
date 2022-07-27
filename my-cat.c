/* my-
wcat: minimalist cat tool print file content in stdout
Parameter argv contains 1 or more files.
Error messages are printed in stdout as well since it was required.
*/

#define _DEFAULT_SOURCE || _POSIX_C_SOURCE >= 200809L /* For getline */

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    /* Check if at least 1 file given, do nothing otherwise. */
    if (argc > 1) {

        /* Initialize variables */
        FILE *fp;
        char *line;
        size_t len = 0;
        size_t read;

        /* Read each file in given order */
        for (int i=1; i<argc; ++i) {
            if ((fp = fopen(argv[i], "r")) == NULL) {
                printf("wcat: cannot open file\n");
                exit(1);
            }
            while ((read = getline(&line, &len, fp)) != -1) printf("%s", line);
            fclose(fp);
        }
        /* free used memory */
        free(line);
    }
    return 0;
}
