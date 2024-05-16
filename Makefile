CC := gcc
CFLAGS := -Wall -Wextra -O2

BIN := rpg


.PHONY: clean


${BIN}: rpg.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm -f ${BIN}
