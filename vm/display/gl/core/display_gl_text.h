#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H
#include <stdarg.h>

struct display_gl_text_s;
typedef struct display_gl_text_s display_gl_text_t;

display_gl_text_t* display_gl_text_intialize();
void			display_gl_text_destroy(display_gl_text_t* texts);
float			display_gl_text_add(display_gl_text_t* texts, float x, float y, int32 rgba, char* format, ...);

float			display_gl_text_add_va(display_gl_text_t* texts, float x, float y, int32 rgba, char* format, va_list args);

void			display_gl_text_render(display_gl_text_t* texts, mat4_t* projection_view);
void			display_gl_text_clear(display_gl_text_t* texts);
#endif
