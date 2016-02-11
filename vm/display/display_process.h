#ifndef __DISPLAY_PROCESS_H__
#define __DISPLAY_PROCESS_H__

struct vm_s;
struct display_s;

void display_process_init(struct display_s* display);
void display_process_update(struct vm_s* vm, struct display_s* display);
void display_process_render(struct vm_s* vm, struct display_s* display);
void display_process_destroy(struct display_s* display);

#endif
