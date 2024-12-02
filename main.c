#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "globals.h"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 1024

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define FPS 60
#define FRAME_TIME (1000 / FPS) 

#define MOVE_SPEED 40

int init_sdl(global_t* state);
int init_state(global_t* state);
int init_maze(global_t* state, size_t cols, size_t rows);
 
void process_input(global_t* state);
void update(global_t* state);
void render(global_t* state);

void restart_game(global_t* state);

int main(int argc, char** argv) {
  if(argc < 3) {
    fprintf(stderr, "Usage: ./main <maze_width> <maze_height>");
    return 1;
  }

  size_t cols = atoi(argv[1]);
  size_t rows = atoi(argv[2]);

  srand(time(NULL));

  global_t state;
  if(init_sdl(&state) || init_state(&state) || init_maze(&state, cols, rows)) return 1;

  while(state.game_runnning_) {
    process_input(&state);
    update(&state); 
    render(&state);
  }

  SDL_DestroyWindow(state.window_);
  SDL_DestroyRenderer(state.renderer_);
  SDL_Quit();

  return 0;
}

void process_input(global_t* state) {
  SDL_PollEvent(&state->e_);
  switch(state->e_.type) {
    case SDL_QUIT:
      state->game_runnning_ = false;
      break;
    case SDL_KEYDOWN:
      if(state->e_.key.keysym.sym == SDLK_ESCAPE) {
        state->game_runnning_ = false;
      } else {
        switch(state->e_.key.keysym.sym) {
          case SDLK_UP:
            update_player(&state->player_, &state->maze_, 0, -1);
            break;
          case SDLK_DOWN:
            update_player(&state->player_, &state->maze_, 0, 1);
            break;
          case SDLK_LEFT:
            update_player(&state->player_, &state->maze_, -1, 0);
            break;
          case SDLK_RIGHT:
            update_player(&state->player_, &state->maze_, 1, 0);
            break;
        }
      }
      break;
  }
}

void update(global_t* state) {
  int to_wait = FRAME_TIME - (SDL_GetTicks()-state->last_frame_time_);
  if(to_wait > 0 && to_wait < FRAME_TIME) SDL_Delay(to_wait);

  float delta = (SDL_GetTicks() - state->last_frame_time_) / 1000.0f;

  state->last_frame_time_ = SDL_GetTicks();

  state->rect_.x += state->rect_.x_d * MOVE_SPEED;
  state->rect_.y += state->rect_.y_d * MOVE_SPEED;
  
  if(state->player_.x_ == state->maze_.cols_-1 
    && state->player_.y_ == state->maze_.rows_-1) restart_game(state);

  // clear rect
  state->rect_.x_d = 0;
  state->rect_.y_d = 0;
}

void render(global_t* state) {
  SDL_SetRenderDrawColor(state->renderer_, 204, 204, 255, 255);
  SDL_RenderClear(state->renderer_);

  render_maze(state->renderer_, &state->maze_);
  render_player(state->renderer_, &state->player_);

  SDL_RenderPresent(state->renderer_);
}

rect_t create_rect(float x, float y, float w, float h) {
  rect_t r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

int init_state(global_t* state) {
  state->game_runnning_ = true;
  state->rect_ = create_rect(20, 20, 20, 20);

  return 0;
}

int init_maze(global_t* state, size_t cols, size_t rows) {
  size_t cell_size = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / MAX(cols, rows);

  int diff_y = SCREEN_HEIGHT / cell_size - rows;
  int diff_x = SCREEN_WIDTH / cell_size - cols;
  
  size_t start_offset_y = diff_y > 0 ? diff_y*cell_size/2: 0;
  size_t start_offset_x = diff_x > 0 ? diff_x*cell_size/2: 0;

  printf("diff_x: %d, diff_y: %d | o_x: %lu, o_y: %lu\n", diff_x, diff_y, start_offset_x, start_offset_y);

  state->maze_ = create_maze(cols, rows, start_offset_x, start_offset_y);
  generate_maze(&state->maze_);
  
  state->player_ = create_player(cell_size, 0, 0, start_offset_x, start_offset_y);

  state->start_offset_x_ = start_offset_x;
  state->start_offset_y_ = start_offset_y;

  return 0;
}

int init_sdl(global_t* state) {
  if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "SDL_INIT error\n");
    return 1;
  }

  state->window_ = SDL_CreateWindow(
    "MAZE", 
    SDL_WINDOWPOS_CENTERED, 
    SDL_WINDOWPOS_CENTERED, 
    SCREEN_WIDTH, 
    SCREEN_HEIGHT, 
    SDL_WINDOW_SHOWN
  );

  if(!state->window_) {
    fprintf(stderr, "Create window error\n");
    return 1;
  }

  // state->surface_ = SDL_GetWindowSurface(state->window_);

  state->renderer_ = SDL_CreateRenderer(state->window_, -1, 0);
  if(!state->renderer_) {
    fprintf(stderr, "Create renderer error\n");
    return 1;
  }

  return 0;
}

void restart_game(global_t* state) {
  size_t cols = state->maze_.cols_;
  size_t rows = state->maze_.rows_;

  free_maze(&state->maze_);

  state->maze_ = create_maze(cols, rows, state->start_offset_x_, state->start_offset_y_);
  generate_maze(&state->maze_);
  
  state->player_.x_ = 0;
  state->player_.y_ = 0;
}
