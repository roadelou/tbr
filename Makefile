HEAD = tbr.h error.h log.h
SRC = main.c tbr.c error.c log.c

debug_tbr.elf: $(HEAD) $(SRC)
	clang -g -Wall -Wextra -o debug_tbr.elf $(SRC)

tbr.elf: $(HEAD) $(SRC)
	clang -O3 -Wall -Wextra -o tbr.elf $(SRC)

install: tbr.elf
	cp tbr.elf /home/roadelou/localApp/tbr

clean:
	rm -f *.elf
