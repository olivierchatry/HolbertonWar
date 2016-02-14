#include <stdlib.h>

#include "core/display_gl_def.h"


typedef struct s_grid_vertex
{
	v3_t	v;
	float	i;
}	t_grid_vertex;

void display_gl_grid_get_position(struct display_gl_s* display, int addr, v3_t* v) {
	int32 width = display->grid_width;
	int32 x = addr % width;
	int32 y = (addr - x) / width;

	v->z = 0;
	v->x = DISPLAY_CELL_SIZE * x + DISPLAY_CELL_SIZE * 0.5f;
	v->y = DISPLAY_CELL_SIZE * y + DISPLAY_CELL_SIZE * 0.5f;
}


void display_gl_grid_init(display_gl_t* display, int memory_size) {
	float						sqr_size = sqrtf(VM_MEMORY_SIZE);
	int32						width = (int32)roundf(sqr_size * display->screen_ratio);
	int32						height = (int32)roundf(sqr_size * 1.0f / display->screen_ratio);
	int32						size = width * height;
	int32						x, y;
	t_grid_vertex*	vb;
	t_grid_vertex*	temp_vb;
	uint16*					ib;
	uint16*					temp_ib;
	int32						vb_size = (size) * 4 * sizeof(t_grid_vertex);
	int32						ib_size = (size) * 6 * sizeof(int16);
	int32						cell_index = 0;

	display->grid_width = width;
	display->grid_height = height;

	temp_vb = vb = malloc(vb_size);
	temp_ib = ib = malloc(ib_size);
	for (y = 0; y < height; ++y)
	{
		float yc = DISPLAY_CELL_SIZE * y;
		float yn = yc + DISPLAY_CELL_SIZE;
		for (x = 0; x < width; ++x)
		{
			float xc = DISPLAY_CELL_SIZE * x;
			float xn = xc + DISPLAY_CELL_SIZE;
			v3_set(&vb->v, xc, yc, 0.f), vb->i = 0, vb++;
			v3_set(&vb->v, xn, yc, 0.f), vb->i = 1, vb++;
			v3_set(&vb->v, xc, yn, 0.f), vb->i = 2, vb++;
			v3_set(&vb->v, xn, yn, 0.f), vb->i = 3, vb++;

			*ib++ = cell_index + 0;
			*ib++ = cell_index + 1;
			*ib++ = cell_index + 3;

			*ib++ = cell_index + 0;
			*ib++ = cell_index + 3;
			*ib++ = cell_index + 2;

			cell_index += 4;
		}
	}
	display->grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->grid_index_buffer = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, ib_size, GL_STATIC_DRAW, temp_ib);
	free(temp_vb);
	free(temp_ib);
}

void display_gl_grid_destroy(struct display_gl_s* display) {
	glDeleteBuffers(1, &display->grid_vertex_buffer);
	glDeleteBuffers(1, &display->grid_index_buffer);
}
