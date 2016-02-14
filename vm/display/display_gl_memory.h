#ifndef __DISPLAY_MEMORY_H__
#define __DISPLAY_MEMORY_H__

struct vm_s;
struct display_gl_s;

void display_gl_memory_init(struct display_gl_s* display);
void display_gl_memory_update(struct vm_s* vm, struct display_gl_s* display);
void display_gl_memory_render(struct vm_s* vm, struct display_gl_s* display);
void display_gl_memory_destroy(struct display_gl_s* display);

#endif
