#ifndef __DISPLAY_GRID_H__
#define __DISPLAY_GRID_H__

#include "core/display_gl_math.h"

struct display_gl_s;

void display_gl_grid_get_position(struct display_gl_s* display, int addr, v3_t* v);
void display_gl_grid_init(struct display_gl_s* display, int memory_size);
void display_gl_grid_destroy(struct display_gl_s* display);
void display_gl_grid_get_height();

#endif
