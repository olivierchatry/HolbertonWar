#ifndef __DISPLAY_PROCESS_H__
#define __DISPLAY_PROCESS_H__

struct vm_s;
struct display_gl_s;

void display_gl_process_init(struct display_gl_s* display);
void display_gl_process_update(struct vm_s* vm, struct display_gl_s* display);
void display_gl_process_render(struct vm_s* vm, struct display_gl_s* display);
void display_gl_process_destroy(struct display_gl_s* display);

#endif
