#ifndef PLAYER_H
#define PLAYER_H

#include "maze.h"
#include <SDL2/SDL.h>

typedef struct playert_t_ {
  size_t x_;
  size_t y_;
  size_t sz_;
  size_t start_offset_x_;
  size_t start_offset_y_;
} player_t;


player_t create_player(size_t sz, size_t x, size_t y, size_t start_off_x, size_t start_off_y);

void update_player(player_t* player, maze_t* maze, int x, int y);
void render_player(SDL_Renderer* renderer, player_t* player);

#endif
