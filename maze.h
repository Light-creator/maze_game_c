#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdbool.h>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 1024

typedef enum dir_t_ {
  DOWN_ = 0, 
  UP_ = 1, 
  RIGHT_ = 2,
  LEFT_ = 3,
  NONE_
} dir_t;

typedef struct cell_t_ {
  size_t x_, y_;
  bool walls_[4];
  bool visited_;
} cell_t;

typedef struct maze_t_ {
  size_t cols_;
  size_t rows_;
  cell_t** cells_;
} maze_t;

maze_t create_maze(size_t cols, size_t rows);
void free_maze(maze_t* maze);

void recursive_backtracking(maze_t* maze, size_t x, size_t y);

void render_maze(SDL_Renderer* renderer, maze_t* maze);
void update_maze(maze_t* maze);

void generate_maze(maze_t* maze);

#endif
