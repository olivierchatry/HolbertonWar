#ifndef __DISPLAY_GRID_H__
#define __DISPLAY_GRID_H__

#include "display_math.h"

struct display_s;

void display_grid_get_position(struct display_s* display, int addr, v3_t* v);
void display_grid_init(struct display_s* display, int memory_size);
void display_grid_destroy(struct display_s* display);
void display_grid_get_height();

#endif
