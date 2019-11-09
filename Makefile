CC = gcc
EXE = rpg
FLAGS = -Wall -Wextra -pedantic -march=native -O2


$(EXE):
	$(CC) $(FLAGS) -o $@ rpg.c
