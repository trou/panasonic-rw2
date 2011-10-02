CC=gcc
COPTS=-W -Wall -Werror -std=c90 -O2
all:
	$(CC) -o parse parse.c
	$(CC) -o parseca parseca.c
	$(CC) -o dng parsedng.c
clean:
	rm -f parse \
	rm -f parseca \
	rm -f dng
