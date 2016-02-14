#ifndef __DISPLAY_JUMP_H__
#define __DISPLAY_JUMP_H__

struct vm_s;
struct display_gl_s;

void display_gl_jump_init(struct display_gl_s* display);
void display_gl_jump_update(struct vm_s* vm, struct display_gl_s* display);
void display_gl_jump_render(struct vm_s* vm, struct display_gl_s* display);
void display_gl_jump_destroy(struct display_gl_s* display);


#endif
