#ifndef __DISPLAY_JUMP_H__
#define __DISPLAY_JUMP_H__

struct vm_s;
struct display_s;

void display_jump_init(struct display_s* display);
void display_jump_update(struct vm_s* vm, struct display_s* display);
void display_jump_render(struct vm_s* vm, struct display_s* display);
void display_jump_destroy(struct display_s* display);


#endif
