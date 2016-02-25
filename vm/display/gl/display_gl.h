#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdarg.h>
#include	"../../common/holberton-core.h"


#define DISPLAY_CELL_SIZE				10.f
#define DISPLAY_OK						1


struct display_gl_s;
struct vm_s;
struct mat4_s;
struct ring_buffer_s;
struct display_gl_text_s;


typedef struct display_gl_s display_gl_t;
typedef struct mat4_s mat4_t;
typedef struct ring_buffer_s ring_buffer_t;

float					display_gl_text(display_gl_t* display, float x, float y, int32 rgba, char* format, ...);
void*					display_gl_get_window(display_gl_t* display);
display_gl_t*	display_gl_initialize(int width, int height, int full_screen);
int32					display_gl_should_exit(display_gl_t* display);
void					display_gl_destroy(display_gl_t* display);
void					display_gl_step(struct vm_s* vm, display_gl_t* display);
int32					display_gl_update_input(display_gl_t* display);
void					display_gl_swap(display_gl_t* display);

#endif
