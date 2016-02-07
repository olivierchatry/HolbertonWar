#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdarg.h>
#include	"../../common/holberton-core.h"


#define DISPLAY_CELL_SIZE				10.f
#define DISPLAY_OK						1


struct display_s;
struct vm_s;
struct mat4_s;
struct ring_buffer_s;
struct display_text_s;


typedef struct display_s display_t;
typedef struct mat4_s mat4_t;
typedef struct ring_buffer_s ring_buffer_t;

float				display_text(display_t* display, float x, float y, int32 rgba, char* format, ...);

display_t*	display_initialize(int width, int height, int full_screen);
int32				display_should_exit(display_t* display);
void				display_destroy(display_t* display);
void				display_step(struct vm_s* vm, display_t* display);
int32				display_update_input(display_t* display);

#endif
