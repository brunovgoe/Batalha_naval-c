CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2
LDFLAGS = 

SRC = src/board.c src/fleet.c src/game.c src/io.c src/rnd.c src/stats.c src/main.c
OBJ = $(SRC:.c=.o)
BIN = batalha_naval.exe

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJ) $(BIN)

.PHONY: all clean
