#include "maze.h"
#include "stack.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAZE_PTR_AT(m, i, j) (m)->cells_[(i)*(m)->cols_ + (j)]
#define MAZE_STATIC_AT(m, i, j) (m).cells_[(i)*(m).cols_ + (j)]

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define OPTIMIZED 1

cell_t* create_cell(size_t x, size_t y) {
  cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
  cell->x_ = x;
  cell->y_ = y;
  cell->visited_ = false;
  memset(cell->walls_, true, sizeof(bool)*4);

  return cell;
}

maze_t create_maze(size_t cols, size_t rows, size_t start_off_x, size_t start_off_y) {
  maze_t maze;
  maze.cols_ = cols;
  maze.rows_ = rows;
  maze.cells_ = (cell_t**)malloc(sizeof(cell_t*)*cols*rows);

  maze.start_offset_x_ = start_off_x;
  maze.start_offset_y_ = start_off_y;
  
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

#ifdef OPTIMIZED
inline bool is_valid_cell(maze_t* maze, int x, int y) {
  if(x < 0 || x >= maze->cols_ || y < 0 || y >= maze->rows_) return false;
  return !MAZE_PTR_AT(maze, y, x)->visited_;
}

inline void shuffle_arr(dir_t* arr, int sz) {
  for(int i=0; i<sz; i++) {
    int rand_idx = rand()%sz;
    dir_t tmp = arr[rand_idx];
    arr[rand_idx] = arr[i];
    arr[i] = tmp;
  }
}
#else
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
#endif

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


void iterative_backtracking(maze_t* maze, size_t x, size_t y) {
  size_t prev_x = x, prev_y = y;
  size_t count = maze->rows_*maze->cols_-1;
  stack_t stack = create_stack(1024);
  
  while(count) {
    MAZE_PTR_AT(maze, y, x)->visited_ = true;
    dir_t dir = get_direction(maze, x, y);
    if(dir != NONE_) {
      push_stack(&stack, MAZE_PTR_AT(maze, y, x));
      count--;
    }

    switch(dir) {
      case UP_:
        MAZE_PTR_AT(maze, y, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y-1, x)->walls_[DOWN_] = false;
        y--;
        break;
      case DOWN_:
        MAZE_PTR_AT(maze, y+1, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y, x)->walls_[DOWN_] = false;
        y++;
        break;
      case RIGHT_:
        MAZE_PTR_AT(maze, y, x)->walls_[RIGHT_] = false;
        MAZE_PTR_AT(maze, y, x+1)->walls_[LEFT_] = false;
        x++;
        break;
      case LEFT_:
        MAZE_PTR_AT(maze, y, x)->walls_[LEFT_] = false;
        MAZE_PTR_AT(maze, y, x-1)->walls_[RIGHT_] = false;
        x--;
        break;
      case NONE_:
        x = stack.cells_[stack.sz_-1]->x_;
        y = stack.cells_[stack.sz_-1]->y_;
        pop_stack(&stack);
        break;
      default: break;
    }
  }
}

void recursive_backtracking(maze_t* maze, size_t x, size_t y) {
  dir_t dir = get_direction(maze, x, y);
  
  MAZE_PTR_AT(maze, y, x)->visited_ = true;
  while(dir != NONE_) {
    switch(dir) {
      case UP_:
        MAZE_PTR_AT(maze, y, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y-1, x)->walls_[DOWN_] = false;
        recursive_backtracking(maze, x, y-1);
        break;
      case DOWN_:
        MAZE_PTR_AT(maze, y+1, x)->walls_[UP_] = false;
        MAZE_PTR_AT(maze, y, x)->walls_[DOWN_] = false;
        recursive_backtracking(maze, x, y+1);
        break;
      case RIGHT_:
        MAZE_PTR_AT(maze, y, x)->walls_[RIGHT_] = false;
        MAZE_PTR_AT(maze, y, x+1)->walls_[LEFT_] = false;
        recursive_backtracking(maze, x+1, y);
        break;
      case LEFT_:
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
#ifdef OPTIMIZED 
  iterative_backtracking(maze, 0, 0);
#else
  recursive_backtracking(maze, 0, 0);
#endif
}

static bool flag = true;

void render_maze(SDL_Renderer* renderer, maze_t* maze) {
  size_t cell_size = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / MAX(maze->cols_, maze->rows_);
  
  SDL_SetRenderDrawColor(renderer, 255,181,192, 255);
  
  SDL_Rect start_point = { 
    maze->start_offset_x_, 
    maze->start_offset_y_, 
    cell_size, 
    cell_size 
  };

  SDL_Rect finish_point = { 
    maze->start_offset_x_+(maze->cols_-1)*cell_size, 
    maze->start_offset_y_+(maze->rows_-1)*cell_size, 
    cell_size, 
    cell_size 
  };

  SDL_RenderFillRect(renderer, &start_point);
  SDL_RenderFillRect(renderer, &finish_point);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  
  int o_x = maze->start_offset_x_;
  int o_y = maze->start_offset_y_;

  clock_t begin = clock();
#ifdef OPTIMIZED
  for(int i=0; i<maze->rows_; i++) {
    int idx_y_first = o_y+i*cell_size;
    int idx_y_second = o_y+(i+1)*cell_size;
    for(int j=0; j<maze->cols_; j++) {
      if(MAZE_PTR_AT(maze, i, j)->walls_[UP_]) {
        SDL_RenderDrawLine(renderer, o_x+j*cell_size, idx_y_first, o_x+(j+1)*cell_size, idx_y_first);
      }
      if(MAZE_PTR_AT(maze, i, j)->walls_[LEFT_]) {
        SDL_RenderDrawLine(renderer, o_x+j*cell_size, idx_y_first, o_x+j*cell_size, idx_y_second);
      }
    }
  }

  int last_col = maze->cols_-1;
  for(int i=0; i<maze->rows_; i++) {
    if(MAZE_PTR_AT(maze, i, last_col)->walls_[RIGHT_]) {
      SDL_RenderDrawLine(renderer, o_x+(last_col+1)*cell_size, o_y+i*cell_size, o_x+(last_col+1)*cell_size, o_y+(i+1)*cell_size);
    }
  }

  int last_row = maze->rows_-1;
  for(int j=0; j<maze->cols_; j++) {
    if(MAZE_PTR_AT(maze, last_row, j)->walls_[DOWN_]) {
      SDL_RenderDrawLine(renderer, o_x+j*cell_size, o_y+(last_row+1)*cell_size, o_x+(j+1)*cell_size, o_y+(last_row+1)*cell_size);
    }
  }

#else
  for(int i=0; i<maze->cols_; i++) {
    for(int j=0; j<maze->rows_; j++) {
      int idx_x_first = o_x+i*cell_size;
      int idx_y_first = o_y+j*cell_size;
      int idx_x_second = o_x+(i+1)*cell_size;
      int idx_y_second = o_y+(j+1)*cell_size;

      if(MAZE_PTR_AT(maze, j, i)->walls_[UP_]) {
        SDL_RenderDrawLine(renderer, idx_x_first, idx_y_first, idx_x_second, idx_y_first);
      }
      if(MAZE_PTR_AT(maze, j, i)->walls_[DOWN_]) {
        SDL_RenderDrawLine(renderer, idx_x_first, idx_y_second, idx_x_second, idx_y_second);
      }
      if(MAZE_PTR_AT(maze, j, i)->walls_[RIGHT_]) {
        SDL_RenderDrawLine(renderer, idx_x_second, idx_y_first, idx_x_second, idx_y_second);
      }
      if(MAZE_PTR_AT(maze, j, i)->walls_[LEFT_]) {
        SDL_RenderDrawLine(renderer, idx_x_first, idx_y_first, idx_x_first, idx_y_second);
      }
    }
  }

#endif


  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  if(flag) {
    printf("Maze size of %lux%lu => Render time: %f\n", maze->rows_, maze->cols_, time_spent);
    flag = false;
  }
}

void update_maze(maze_t* maze) {

}
