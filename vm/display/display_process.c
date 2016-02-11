#include <stdlib.h>
#include <string.h>

#include "core/display_def.h"
#include "core/display_mesh_generator.h"

#include "display_memory.h"
#include "display_grid.h"

#include "../vm.h"

void display_process_init(struct display_s* display) {
	int32								vb_count;
	int32								ib_count;
	int8*								vb;
	uint16* 						ib;
	v3_t 								center;
	float 							radius = DISPLAY_CELL_SIZE * 0.25f;
	t_mesh_definition* 	def;
	int32 							vb_size;
	int32 							count = 6;
	int32 							sub_division = 16;

	def = display_mesh_get_definiton(MESH_TYPE_VN);
	display_generate_sphere_count(sub_division, &vb_count, &ib_count);

	vb_size = vb_count * def->stride;

	vb = malloc(vb_size * count);
	ib = malloc(ib_count * sizeof(uint16) * count);

	v3_set(&center, radius, 0.0f, 0.0f);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb, def, ib, 0);
	v3_set(&center, -radius, 0.0f, 0.0f);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size, def, ib + ib_count, vb_count);
	v3_set(&center, 0.0f, -radius, 0.0f);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 2, def, ib + ib_count * 2, vb_count * 2);
	v3_set(&center, 0.0f, radius, 0.0f);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 3, def, ib + ib_count * 3, vb_count * 3);
	v3_set(&center, 0.0f, 0.0f, -radius);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 4, def, ib + ib_count * 4, vb_count * 4);
	v3_set(&center, 0.0f, 0.0f, radius);
	display_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 5, def, ib + ib_count * 5, vb_count * 5);

	display->process_mesh = display_mesh_vn_create(vb, vb_count * count, ib, ib_count * count);

	free(vb);
	free(ib);
}

void display_process_update(struct vm_s* vm, struct display_s* display) {

}


void display_process_render(struct vm_s* vm, struct display_s* display) {
	v4_t	color_io_process;
	v4_t	color_ambient;
	v3_t	light_direction;
	mat4_t	local;
	mat4_t	translate;
	mat4_t	rotation;
	quat_t	quat;
	int		i;

	v4_set(&color_io_process, 0.4f, 0.4f, 1.0f, 0.0f);
	v4_set(&color_ambient, 0.2f, 0.2f, 0.2f, 1.0f);
	v3_set(&light_direction, 0, 0, 1.0f);


	display_mesh_render_start(display->mesh_renderer, MESH_TYPE_VN);
	display_mesh_set_ambient(display->mesh_renderer, &color_ambient);
	display_mesh_set_light_direction(display->mesh_renderer, &light_direction);
	display_mesh_set_diffuse(display->mesh_renderer, &color_io_process);
	display_mesh_set_projection(display->mesh_renderer, &display->projection_view);
	mat4_ident(&local);

	for (i = 0; i < vm->process_count; ++i)
	{
		process_t* process = vm->processes[i];
		int index = process->pc;

		if (vm->shadow[index] < 8) {
			float angle = (float)process->cycle_create * ((float)display->frame_last_time / (float)vm->cycle_total);
			mat4_t normal;
			float x = (float)(index % display->memory_stride);
			float y = (float)(index / display->memory_stride);

			vm->shadow[index]++;

			x = x * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;
			y = y * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;

			mat4_ident(&translate);
			mat4_translate(&translate, x, y, DISPLAY_CELL_SIZE * 0.5f);

			quat_from_euler(&quat, angle, angle, angle);
			quat_to_mat4(&quat, &rotation);
			mat4_invert(&rotation, &normal);
			mat4_transpose(&normal, &normal);
			mat4_mul(&translate, &rotation, &local);

			display_mesh_set_local(display->mesh_renderer, &local);
			display_mesh_set_normal(display->mesh_renderer, &normal);

			display_mesh_render(display->process_mesh);
		}
	}
}


void display_process_destroy(struct display_s* display) {
	display_mesh_destroy(display->process_mesh);
}
