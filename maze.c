#include "maze.h"

#include <stdlib.h>
#include <string.h>

#define MAZE_PTR_AT(m, i, j) (m)->cells_[(i)*(m)->cols_ + (j)]
#define MAZE_STATIC_AT(m, i, j) (m).cells_[(i)*(m).cols_ + (j)]

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

cell_t* create_cell(size_t x, size_t y) {
  cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
  cell->x_ = x;
  cell->y_ = y;
  cell->visited_ = false;
  memset(cell->walls_, true, sizeof(bool)*4);

  return cell;
}

maze_t create_maze(size_t cols, size_t rows) {
  maze_t maze;
  maze.cols_ = cols;
  maze.rows_ = rows;
  maze.cells_ = (cell_t**)malloc(sizeof(cell_t*)*cols*rows);
  
  for(int i=0; i<rows; i++) {
    for(int j=0; j<cols; j++)
      MAZE_STATIC_AT(maze, i, j) = create_cell(j, i);  
  }

  return maze;
}

void free_maze(maze_t* maze) {
  for(int i=0; i<maze->rows_; i++) {
    for(int j=0; j<maze->cols_; j++) 
      free(MAZE_PTR_AT(maze, i, j));
  }

  free(maze->cells_);
}

bool is_valid_cell(maze_t* maze, int x, int y) {
  if(x < 0 || x >= maze->cols_ || y < 0 || y >= maze->rows_) return false;
  return !MAZE_PTR_AT(maze, y, x)->visited_;
}

void shuffle_arr(dir_t* arr, int sz) {
  for(int i=0; i<sz; i++) {
    int rand_idx = rand()%sz;
    dir_t tmp = arr[rand_idx];
    arr[rand_idx] = arr[i];
    arr[i] = tmp;
  }
}

dir_t get_direction(maze_t* maze, int x, int y) {
  // DOWN_, UP_, RIGHT_, LEFT_
  int y_move[4] = {1, -1, 0,  0};
  int x_move[4] = {0,  0, 1, -1};

  dir_t dirs[4] = {DOWN_, UP_, RIGHT_, LEFT_};
  shuffle_arr(dirs, 4);

  int random_choice = rand() % 4;
  
  for(int i=0; i<4; i++) {
    // printf("%d %d %d %d\n", dirs[i], x_move[dirs[i]], y_move[dirs[i]], 1);
    if(is_valid_cell(maze, x+x_move[dirs[i]], y+y_move[dirs[i]]))
      return dirs[i];
  }

  return NONE_;
}

void recursive_backtracking(maze_t* maze, size_t x, size_t y) {
  printf("(%lu %lu) -> ", x, y);
  dir_t dir = get_direction(maze, x, y);
  // dir_t dir = DOWN_;
  MAZE_PTR_AT(maze, y, x)->visited_ = true;
  while(dir != NONE_) {
    switch(dir) {
      case UP_:
        printf("UP ");
        MAZE_PTR_AT(maze, y, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y-1, x)->walls_[DOWN_] = false;
        recursive_backtracking(maze, x, y-1);
        break;
      case DOWN_:
        printf("DOWN ");
        MAZE_PTR_AT(maze, y+1, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y, x)->walls_[DOWN_] = false;
        recursive_backtracking(maze, x, y+1);
        break;
      case RIGHT_:
        printf("RIGHT ");
        MAZE_PTR_AT(maze, y, x)->walls_[RIGHT_] = false;
        MAZE_PTR_AT(maze, y, x+1)->walls_[LEFT_] = false;
        recursive_backtracking(maze, x+1, y);
        break;
      case LEFT_:
        printf("LEFT ");
        MAZE_PTR_AT(maze, y, x)->walls_[LEFT_] = false;
        MAZE_PTR_AT(maze, y, x-1)->walls_[RIGHT_] = false;
        recursive_backtracking(maze, x-1, y);
        break;
      default: return;
    }

    dir = get_direction(maze, x, y);
  }
}

void generate_maze(maze_t* maze) {
  printf("start\n");
  recursive_backtracking(maze, 0, 0);
}

void render_maze(SDL_Renderer* renderer, maze_t* maze) {
  size_t cell_size = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / MAX(maze->cols_, maze->rows_);
  
  SDL_SetRenderDrawColor(renderer, 255,181,192, 255);
  SDL_Rect start_point = { 0, 0, cell_size, cell_size };
  SDL_Rect finish_point = { (maze->cols_-1)*cell_size, (maze->rows_-1)*cell_size, cell_size, cell_size };
  SDL_RenderFillRect(renderer, &start_point);
  SDL_RenderFillRect(renderer, &finish_point);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for(int i=0; i<maze->rows_; i++) {
    for(int j=0; j<maze->cols_; j++) {
      if(MAZE_PTR_AT(maze, i, j)->walls_[UP_]) {
        SDL_RenderDrawLine(renderer, j*cell_size, i*cell_size, (j+1)*cell_size, i*cell_size);
      }
      if(MAZE_PTR_AT(maze, i, j)->walls_[DOWN_]) {
        SDL_RenderDrawLine(renderer, j*cell_size, (i+1)*cell_size, (j+1)*cell_size, (i+1)*cell_size);
      }
      if(MAZE_PTR_AT(maze, i, j)->walls_[RIGHT_]) {
        SDL_RenderDrawLine(renderer, (j+1)*cell_size, i*cell_size, (j+1)*cell_size, (i+1)*cell_size);
      }
      if(MAZE_PTR_AT(maze, i, j)->walls_[LEFT_]) {
        SDL_RenderDrawLine(renderer, j*cell_size, i*cell_size, j*cell_size, (i+1)*cell_size);
      }
    }
  }
}

void update_maze(maze_t* maze) {

}
