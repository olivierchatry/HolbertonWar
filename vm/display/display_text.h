#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H


struct display_text_s;
typedef struct display_text_s display_text_t;

display_text_t* display_text_intialize();
void			display_text_destroy(display_text_t* texts);
float			display_text_add(display_text_t* texts, float x, float y, int32 rgba, char* format, ...);

float			display_text_add_va(display_text_t* texts, float x, float y, int32 rgba, char* format, va_list args);

void			display_text_render(display_text_t* texts, mat4_t* projection_view);
void			display_text_clear(display_text_t* texts);
#endif