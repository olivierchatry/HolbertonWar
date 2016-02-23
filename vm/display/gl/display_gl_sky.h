#ifndef __DISPLAY_SKY_H__
#define __DISPLAY_SKY_H__

struct vm_s;
struct display_gl_s;

void display_gl_sky_init(struct display_gl_s* display);
void display_gl_sky_update(struct vm_s* vm, struct display_gl_s* display);
void display_gl_sky_render(struct vm_s* vm, struct display_gl_s* display);
void display_gl_sky_destroy(struct display_gl_s* display);


#endif
