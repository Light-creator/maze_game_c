#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>

#include "maze.h"
#include "player.h"

typedef struct rect_t_ {
  float x, x_d;
  float y, y_d;
  float w;
  float h;
} rect_t;

typedef struct global_t_ {
  SDL_Window* window_;
  // SDL_Surface* surface_;
  SDL_Renderer* renderer_;
  SDL_Event e_;

  maze_t maze_;
  rect_t rect_;

  player_t player_;

  bool game_runnning_;
  uint32_t last_frame_time_;

  size_t start_offset_x_;
  size_t start_offset_y_;
} global_t;


#endif
