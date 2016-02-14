#ifndef __DISPLAY_IO_H__
#define __DISPLAY_IO_H__


struct vm_s;
struct display_gl_s;

void display_gl_io_init(struct display_gl_s* display);
void display_gl_io_update(struct vm_s* vm, struct display_gl_s* display);
void display_gl_io_render(struct vm_s* vm, struct display_gl_s* display);
void display_gl_io_destroy(struct display_gl_s* display);

#endif
