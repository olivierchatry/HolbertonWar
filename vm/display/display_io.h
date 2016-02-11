#ifndef __DISPLAY_IO_H__
#define __DISPLAY_IO_H__


struct vm_s;
struct display_s;

void display_io_init(struct display_s* display);
void display_io_update(struct vm_s* vm, struct display_s* display);
void display_io_render(struct vm_s* vm, struct display_s* display);
void display_io_destroy(struct display_s* display);

#endif
