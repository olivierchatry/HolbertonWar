#ifndef DISPLAY_H
#define DISPLAY_H


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
typedef struct display_text_s display_text_t;

display_text_t* display_get_text(display_t* display);
display_t*	display_initialize(int width, int height);
int32		display_should_exit(display_t* display);
void		display_destroy(display_t* display);
void		display_step(struct vm_s* vm, display_t* display);
int			display_key_pressed(display_t* display, int key);
int32		display_update_input(display_t* display);
mat4_t*		display_get_projection_view(display_t* display);
void		display_print_ring_buffer(display_t* display, float x, float y, ring_buffer_t* buffer);
#endif
