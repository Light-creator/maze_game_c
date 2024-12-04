#ifndef STACK_H
#define STACK_H

#include "maze.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct stack_t_ {
  cell_t** cells_;
  size_t sz_;
  size_t cap_;
} stack_t;

stack_t create_stack(size_t cap) {
  stack_t stack;
  stack.cap_ = cap;
  stack.sz_ = 0;
  stack.cells_ = (cell_t**)malloc(sizeof(cell_t*)*cap);
  
  return stack;
}

void push_stack(stack_t* stack, cell_t* cell) {
  if(stack->sz_ == stack->cap_) {
    stack->cap_ = stack->cap_ == 0 ? 1 : stack->cap_*2;
    stack->cells_ = (cell_t**)realloc(stack->cells_, sizeof(cell_t*)*stack->cap_);
  } 

  stack->cells_[stack->sz_++] = cell;
}

void pop_stack(stack_t* stack) {
  stack->sz_--;
}

bool stack_empty(stack_t* stack) {
  return stack->sz_ == 0;
}

void free_stack(stack_t* stack) {
  free(stack->cells_);
}

#endif

