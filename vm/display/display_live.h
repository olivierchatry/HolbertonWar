#ifndef __DISPLAY_LIVE_H__
#define __DISPLAY_LIVE_H__

struct vm_s;
struct display_s;

void display_live_init(struct display_s* display);
void display_live_update(struct vm_s* vm, struct display_s* display);
void display_live_render(struct vm_s* vm, struct display_s* display);
void display_live_destroy(struct display_s* display);


#endif
