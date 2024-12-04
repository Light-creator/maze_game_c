CC = gcc

FLAGS = -lSDL2

OUT = ./main

SRCS = \
			 main.c \
			 maze.c \
			 player.c

build_optimized:
	$(CC) $(SRCS) -O2 -o $(OUT) $(FLAGS)

build: 
	$(CC) $(SRCS) -o $(OUT) $(FLAGS)

clean: 
	rm $(OUT)

all: clean build

run:
	$(OUT) $(X) $(Y)

