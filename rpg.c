#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


struct args {
    int passwd_length;
    bool opt_lowercase_letters;
    bool opt_uppercase_letters;
    bool opt_numbers;
    bool opt_special_characters;
};


static const char* LOWERCASE_LETTERS = "abcdefghijklmnopqrstuvwxyz";
static const char* UPPERCASE_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const int SIZE_LETTERS = 26;

static const char* NUMBERS = "0123456789";
static const int SIZE_NUMBERS = 10;

static const char* SPECIAL_CHARACTERS = "!\"#$%&'()*+,-./:;<=>?@[\\]_{|}";
static const int SIZE_SPECIAL_CHARACTERS = 29;

static const int MIN_PASSWD_LENGTH = 1;


static bool parse_args(int argc, char* argv[], struct args* out_args) {
    out_args->passwd_length = -1;
    out_args->opt_lowercase_letters = false;
    out_args->opt_uppercase_letters = false;
    out_args->opt_numbers = false;
    out_args->opt_special_characters = false;

    bool passwd_length_defined = false;

    // check if the number of arguments is valid
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <options> <password length>\n", argv[0]);
        fprintf(stderr, "options:\n");
        fprintf(stderr, "    -l   use lowercase letters\n");
        fprintf(stderr, "    -u   use uppercase letters\n");
        fprintf(stderr, "    -n   use numbers\n");
        fprintf(stderr, "    -s   use special characters\n");
        return false;
    }

    // check if the arguments are valid
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { // options
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                switch (argv[i][j]) {
                    case 'l':
                        out_args->opt_lowercase_letters = true;
                        break;
                    case 'u':
                        out_args->opt_uppercase_letters = true;
                        break;
                    case 'n':
                        out_args->opt_numbers = true;
                        break;
                    case 's':
                        out_args->opt_special_characters = true;
                        break;
                    default:
                        fprintf(stderr, "Invalid option: -%c\n", argv[i][j]);
                        return false;
                }
            }
        } else { // password length
            if (passwd_length_defined) {
                fprintf(stderr, "Invalid argument: %s\n", argv[i]);
                return false;
            } else {
                int passwd_length = atoi(argv[i]);
                if (passwd_length < MIN_PASSWD_LENGTH) {
                    fprintf(stderr, "Invalid password length: %s\n", argv[i]);
                    return false;
                }
                out_args->passwd_length = passwd_length;
                passwd_length_defined = true;
            }
        }
    }

    // if the password length isn't defined
    if (!passwd_length_defined) {
        fprintf(stderr, "Password length not defined\n");
        return false;
    }

    // if no options are defined
    if (
        !out_args->opt_lowercase_letters &&
        !out_args->opt_uppercase_letters &&
        !out_args->opt_numbers &&
        !out_args->opt_special_characters
    ) {
        fprintf(stderr, "No option enabled\n");
        return false;
    }

    return true;
}


static void create_character_pool(const struct args* args, char** out_characters, int* out_size_characters) {
    int size_characters =
            (args->opt_lowercase_letters ? SIZE_LETTERS : 0) +
            (args->opt_uppercase_letters ? SIZE_LETTERS : 0) +
            (args->opt_numbers ? SIZE_NUMBERS : 0) +
            (args->opt_special_characters ? SIZE_SPECIAL_CHARACTERS : 0);

    char* characters = (char*) malloc(size_characters * sizeof(char));

    int i = 0;

    if (args->opt_lowercase_letters) {
        strncpy(characters, LOWERCASE_LETTERS, SIZE_LETTERS);
        i += SIZE_LETTERS;
    }

    if (args->opt_uppercase_letters) {
        strncpy(characters+i, UPPERCASE_LETTERS, SIZE_LETTERS);
        i += SIZE_LETTERS;
    }

    if (args->opt_numbers) {
        strncpy(characters+i, NUMBERS, SIZE_NUMBERS);
        i += SIZE_NUMBERS;
    }

    if (args->opt_special_characters) {
        strncpy(characters+i, SPECIAL_CHARACTERS, SIZE_SPECIAL_CHARACTERS);
    }

    *out_characters = characters;
    *out_size_characters = size_characters;
}


int main(int argc, char* argv[]) {
    // parse arguments
    struct args args;
    if (!parse_args(argc, argv, &args)) {
        return EXIT_FAILURE;
    }

    // create the pool of characters that will be used
    char* characters;
    int size_characters;
    create_character_pool(&args, &characters, &size_characters);

    // open /dev/urandom
    int urandom = open("/dev/urandom", O_RDONLY);
    if (urandom < 0) {
        fprintf(stderr, "Error opening /dev/urandom\n");
        return EXIT_FAILURE;
    }

    // read the random numbers to random_number_buffer[]
    unsigned char random_number_buffer[args.passwd_length];
    if (read(urandom, random_number_buffer, args.passwd_length) <= 0) {
        fprintf(stderr, "Error reading from /dev/urandom\n");
        return EXIT_FAILURE;
    }

    close(urandom);

    // fill passwd[] with the characters from characters[]
    char passwd[args.passwd_length+1];
    for (int i = 0; i < args.passwd_length; ++i) {
        passwd[i] = characters[random_number_buffer[i] % size_characters];
    }

    passwd[args.passwd_length] = '\0';

    // print the result
    puts(passwd);

    // free allocated memory
    free(characters);

    return EXIT_SUCCESS;
}
