#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char* argv[]) {
	// check if the number of arguments is valid
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <options> <length>\n", argv[0]);
		fprintf(stderr, "options:\n");
		fprintf(stderr, "\t-l\tuse lowercase letters\n");
		fprintf(stderr, "\t-u\tuse uppercase letters\n");
		fprintf(stderr, "\t-n\tuse numbers\n");
		fprintf(stderr, "\t-s\tuse special characters\n");
		return 1;
	}

	unsigned int length = 0;	// length of the password
	bool opt_low = false,		// lowercase letters flag
		 opt_upp = false,		// uppercase letters flag
		 opt_num = false,		// numbers flag
		 opt_spc = false;		// special characters flag

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
						return 2;
				}
			}
		} else { // length
			if (length == 0) { // if undefined
				if ((length = atoi(argv[i])) <= 0) {
					fprintf(stderr, "Invalid length: %s\n", argv[i]);
					return 3;
				}
			} else { // if already defined
				fprintf(stderr, "Invalid arguments\n");
				return 4;
			}
		}
	}

	// if length isn't defined
	if (length == 0) {
		fprintf(stderr, "Length not defined\n");
		return 5;
	}

	// if no options are defined
	if (opt_low == false && opt_upp == false && opt_num == false && opt_spc == false) {
		fprintf(stderr, "Options not defined\n");
		return 6;
	}

	const char chr_low[] = "abcdefghijklmnopqrstuvwxyz";
	const char chr_upp[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char chr_num[] = "0123456789";
	const char chr_spc[] = "!\"#$%&'()*+,-./:;<=>?@[\\]_{|}";

	char passwd[length+1];

	// fazer cenas aqui

	passwd[length] = '\0';
	printf("%s\n", passwd);

	return 0;
}
