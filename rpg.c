#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


#define CHR_LOW         "abcdefghijklmnopqrstuvwxyz"
#define CHR_UPP         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define SIZE_LET        26

#define CHR_NUM         "0123456789"
#define SIZE_NUM        10

#define CHR_SPC         "!\"#$%&'()*+,-./:;<=>?@[\\]_{|}"
#define SIZE_SPC        29


static int length = -1;             // length of the password
static bool opt_low = false,        // lowercase letters flag
        opt_upp = false,            // uppercase letters flag
        opt_num = false,            // numbers flag
        opt_spc = false;            // special characters flag


static int parseArgs(int argc, char* argv[]) {
    // check if the number of arguments is valid
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <flags> <length>\n", argv[0]);
        fprintf(stderr, "flags:\n");
        fprintf(stderr, "    -l   use lowercase letters\n");
        fprintf(stderr, "    -u   use uppercase letters\n");
        fprintf(stderr, "    -n   use numbers\n");
        fprintf(stderr, "    -s   use special characters\n");
        return 1;
    }

    // check if the arguments are valid
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { // option
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                switch (argv[i][j]) {
                    case 'l':
                        opt_low = true;
                        break;
                    case 'u':
                        opt_upp = true;
                        break;
                    case 'n':
                        opt_num = true;
                        break;
                    case 's':
                        opt_spc = true;
                        break;
                    default:
                        fprintf(stderr, "Invalid option: -%c\n", argv[i][j]);
                        return 1;
                }
            }
        } else { // length
            if (length == -1) { // if undefined
                if ((length = atoi(argv[i])) <= 0) {
                    fprintf(stderr, "Invalid length: %s\n", argv[i]);
                    return 1;
                }
            } else { // if already defined
                fprintf(stderr, "Invalid argument: %s\n", argv[i]);
                return 1;
            }
        }
    }

    // if length isn't defined
    if (length == -1) {
        fprintf(stderr, "Length not defined\n");
        return 1;
    }

    // if no options are defined
    if (!opt_low && !opt_upp && !opt_num && !opt_spc) {
        fprintf(stderr, "No option defined\n");
        return 1;
    }

    return 0;
}


int main(int argc, char* argv[]) {
    if ( parseArgs(argc, argv) )
        return EXIT_FAILURE;

    // create the buffer with the chosen chars
    unsigned int size_chars =
            (opt_low ? SIZE_LET : 0) +
            (opt_upp ? SIZE_LET : 0) +
            (opt_num ? SIZE_NUM : 0) +
            (opt_spc ? SIZE_SPC : 0);
    char chars[size_chars];
    unsigned int i = 0;
    if ( opt_low ) {
        strncpy(chars+i, CHR_LOW, SIZE_LET);
        i += SIZE_LET;
    }
    if ( opt_upp ) {
        strncpy(chars+i, CHR_UPP, SIZE_LET);
        i += SIZE_LET;
    }
    if ( opt_num ) {
        strncpy(chars+i, CHR_NUM, SIZE_NUM);
        i += SIZE_NUM;
    }
    if ( opt_spc ) {
        strncpy(chars+i, CHR_SPC, SIZE_SPC);
    }


    // open /dev/urandom for the random numbers
    int urand = open("/dev/urandom", O_RDONLY);
    if (urand < 0) {
        fprintf(stderr, "Error opening /dev/urandom\n");
        return EXIT_FAILURE;
    }

    char buf[length];

    // read the random numbers to buf[]
    if (read(urand, buf, length) <= 0) {
        fprintf(stderr, "Error reading from /dev/urandom\n");
        return EXIT_FAILURE;
    }

    close(urand);


    char passwd[length+1];

    // fill passwd[] with the chars from chars[]
    for (i = 0; i < length; ++i)
        passwd[i] = chars[buf[i] % size_chars];

    passwd[length] = '\0';

    printf("%s\n", passwd);

    return EXIT_SUCCESS;
}
