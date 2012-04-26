CC=gcc
WCC=i686-w64-mingw32-gcc
COPTS=-W -Wall -Werror -std=c90 -O2
all:
	$(CC) -o parse parse.c
	$(CC) -o parseca parseca.c
	$(CC) -o dng parsedng.c
win:
	$(WCC) -o parse.exe parse.c
	$(WCC) -o parseca.exe parseca.c
clean:
	rm -f parse \
	rm -f parseca \
	rm -f dng
