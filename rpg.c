#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


typedef struct args {
    int passwd_len;
    bool opt_low_letters;
    bool opt_upp_letters;
    bool opt_numbers;
    bool opt_special_chars;
} Args;


static const int PASSWD_LEN_MIN = 1;

static const char* const LOW_LETTERS = "abcdefghijklmnopqrstuvwxyz";
static const char* const UPP_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const int SIZE_LETTERS = 26;

static const char* const NUMBERS = "0123456789";
static const int SIZE_NUMBERS = 10;

static const char* const SPECIAL_CHARS = "!\"#$%&'()*+,-./:;<=>?@[\\]_{|}";
static const int SIZE_SPECIAL_CHARS = 29;


static void print_help(const char* argv_0) {
    fprintf(stderr, "Usage: %s <options> <password length>\n", argv_0);
    fprintf(stderr, "options:\n");
    fprintf(stderr, "    -l   use lowercase letters\n");
    fprintf(stderr, "    -u   use uppercase letters\n");
    fprintf(stderr, "    -n   use numbers\n");
    fprintf(stderr, "    -s   use special characters\n");
}


static bool parse_options(const char* str, Args* const args) {
    for (int i = 0; str[i] != '\0'; ++i) {
        char option = str[i];

        switch (option) {
            case 'l':
                args->opt_low_letters = true;
                break;
            case 'u':
                args->opt_upp_letters = true;
                break;
            case 'n':
                args->opt_numbers = true;
                break;
            case 's':
                args->opt_special_chars = true;
                break;
            default:
                fprintf(stderr, "Invalid option: -%c\n", option);
                return false;
        }
    }

    return true;
}


static bool parse_passwd_len(const char* str, Args* const args) {
    if (args->passwd_len >= PASSWD_LEN_MIN) {
        fprintf(stderr, "Invalid argument: %s\n", str);
        return false;
    }

    char* str_end;
    long passwd_len = strtol(str, &str_end, 10);

    if (str_end == str || *str_end != '\0' || passwd_len < (long) PASSWD_LEN_MIN) {
        fprintf(stderr, "Invalid password length: %s\n", str);
        return false;
    }

    if (passwd_len > (long) INT_MAX) {
        passwd_len = INT_MAX;
    }

    args->passwd_len = passwd_len;

    return true;
}


static bool parse_args(int argc, char** argv, Args** const out_args) {
    Args* args = (Args*) malloc(sizeof(Args));

    args->passwd_len = 0;
    args->opt_low_letters = false;
    args->opt_upp_letters = false;
    args->opt_numbers = false;
    args->opt_special_chars = false;

    bool success = true;

    // check if the number of arguments is valid
    if (argc < 2) {
        print_help(argv[0]);
        success = false;
    }

    // check if the arguments are valid
    for (int i = 1; success && i < argc; ++i) {
        char* argv_i = argv[i];

        if (argv_i[0] == '-') {
            // options
            if (!parse_options(argv_i+1, args)) {
                success = false;
            }
        } else {
            // password length
            if (!parse_passwd_len(argv_i, args)) {
                success = false;
            }
        }
    }

    // if the password length isn't defined
    if (success && args->passwd_len < PASSWD_LEN_MIN) {
        fprintf(stderr, "Password length not defined\n");
        success = false;
    }

    // if no options are defined
    if (
        success &&
        !args->opt_low_letters &&
        !args->opt_upp_letters &&
        !args->opt_numbers &&
        !args->opt_special_chars
    ) {
        fprintf(stderr, "No option enabled\n");
        success = false;
    }

    // if there's an error
    if (!success) {
        free(args);
        return false;
    }

    *out_args = args;
    return true;
}


static void create_char_pool(const Args* args, char** const out_char_pool, uint8_t* const out_size_char_pool) {
    uint8_t size_char_pool =
            (args->opt_low_letters ? SIZE_LETTERS : 0) +
            (args->opt_upp_letters ? SIZE_LETTERS : 0) +
            (args->opt_numbers ? SIZE_NUMBERS : 0) +
            (args->opt_special_chars ? SIZE_SPECIAL_CHARS : 0);

    char* char_pool = (char*) malloc(size_char_pool * sizeof(char));

    int i = 0;

    if (args->opt_low_letters) {
        strncpy(char_pool, LOW_LETTERS, SIZE_LETTERS);
        i += SIZE_LETTERS;
    }

    if (args->opt_upp_letters) {
        strncpy(char_pool+i, UPP_LETTERS, SIZE_LETTERS);
        i += SIZE_LETTERS;
    }

    if (args->opt_numbers) {
        strncpy(char_pool+i, NUMBERS, SIZE_NUMBERS);
        i += SIZE_NUMBERS;
    }

    if (args->opt_special_chars) {
        strncpy(char_pool+i, SPECIAL_CHARS, SIZE_SPECIAL_CHARS);
    }

    *out_char_pool = char_pool;
    *out_size_char_pool = size_char_pool;
}


int main(int argc, char** argv) {
    // parse arguments
    Args* args;
    if (!parse_args(argc, argv, &args)) {
        return EXIT_FAILURE;
    }

    // create the pool of characters that will be used
    char* char_pool;
    uint8_t size_char_pool;
    create_char_pool(args, &char_pool, &size_char_pool);

    // open /dev/urandom
    int urandom = open("/dev/urandom", O_RDONLY);
    if (urandom < 0) {
        fprintf(stderr, "Error opening /dev/urandom\n");
        return EXIT_FAILURE;
    }

    // read random bytes
    uint32_t random_numbers[args->passwd_len];
    if (read(urandom, random_numbers, args->passwd_len * sizeof(uint32_t)) <= 0) {
        fprintf(stderr, "Error reading from /dev/urandom\n");
        return EXIT_FAILURE;
    }

    // close /dev/urandom
    close(urandom);

    // fill `passwd` with the characters from `char_pool`
    char passwd[args->passwd_len+1];
    double coeff = ((double) size_char_pool) / ((double) UINT32_MAX);

    for (int i = 0; i < args->passwd_len; ++i) {
        uint32_t random_number = random_numbers[i];

        uint8_t char_index = trunc(((double) random_number) * coeff);
        if (char_index == size_char_pool) {
            --char_index;
        }

        passwd[i] = char_pool[char_index];
    }

    passwd[args->passwd_len] = '\0';

    // print the result
    puts(passwd);

    // free allocated memory
    free(args);
    free(char_pool);

    return EXIT_SUCCESS;
}
