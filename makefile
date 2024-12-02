CC = gcc

FLAGS = -lSDL2 -g

OUT = ./main

SRCS = \
			 main.c \
			 maze.c \
			 player.c


build: 
	$(CC) $(SRCS) -o $(OUT) $(FLAGS)

clean: 
	rm $(OUT)

all: clean build

run:
	$(OUT) $(X) $(Y)

