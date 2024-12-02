#include "player.h"

#include <stdlib.h>
#include <SDL2/SDL_render.h>

#define MAZE_PTR_AT(m, i, j) (m)->cells_[(i)*(m)->cols_ + (j)]
#define MAZE_STATIC_AT(m, i, j) (m).cells_[(i)*(m).cols_ + (j)]

player_t create_player(size_t sz, size_t x, size_t y, size_t start_off_x, size_t start_off_y) {
  player_t player;
  player.sz_ = sz;
  player.x_ = x;
  player.y_ = y;
  player.start_offset_x_ = start_off_x;
  player.start_offset_y_ = start_off_y;

  return player;
}

void update_player(player_t* player, maze_t* maze, int x, int y) {
  cell_t* cell = MAZE_PTR_AT(maze, player->y_, player->x_);

  if(x < 0 && player->x_ > 0 && !cell->walls_[LEFT_]) player->x_ += x;
  else if(x > 0 && player->x_ < maze->cols_ && !cell->walls_[RIGHT_]) player->x_ += x;
  else if(y > 0 && player->y_ < maze->rows_ && !cell->walls_[DOWN_]) player->y_ += y;
  else if(y < 0 && player->x_ > 0 && !cell->walls_[UP_]) player->y_ += y;
}

void render_player(SDL_Renderer* renderer, player_t* player) {
  SDL_SetRenderDrawColor(renderer, 52, 21, 57, 255);
  
  SDL_Rect player_rect = {
    player->start_offset_x_+player->x_*player->sz_, 
    player->start_offset_y_+player->y_*player->sz_, 
    player->sz_, 
    player->sz_
  };

  SDL_RenderFillRect(renderer, &player_rect);
}
