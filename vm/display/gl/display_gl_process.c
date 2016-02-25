#include <stdlib.h>
#include <string.h>

#include "core/display_gl_def.h"
#include "core/display_gl_mesh_generator.h"

#include "display_gl_memory.h"
#include "display_gl_grid.h"

#include "../../vm.h"

void display_gl_process_init(struct display_gl_s* display) {
	int32								vb_count;
	int32								ib_count;
	int8*								vb;
	uint16* 						ib;
	v3_t 								center;
	float 							radius = DISPLAY_CELL_SIZE * 0.25f;
	mesh_definition_t* 	def;
	int32 							vb_size;
	int32 							count = 6;
	int32 							sub_division = 16;

	def = display_gl_mesh_get_definiton(MESH_TYPE_VN);
	display_gl_generate_sphere_count(sub_division, &vb_count, &ib_count);

	vb_size = vb_count * def->stride;

	vb = malloc(vb_size * count);
	ib = malloc(ib_count * sizeof(uint16) * count);

	v3_set(&center, radius, 0.0f, 0.0f);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb, def, ib, 0);
	v3_set(&center, -radius, 0.0f, 0.0f);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size, def, ib + ib_count, vb_count);
	v3_set(&center, 0.0f, -radius, 0.0f);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 2, def, ib + ib_count * 2, vb_count * 2);
	v3_set(&center, 0.0f, radius, 0.0f);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 3, def, ib + ib_count * 3, vb_count * 3);
	v3_set(&center, 0.0f, 0.0f, -radius);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 4, def, ib + ib_count * 4, vb_count * 4);
	v3_set(&center, 0.0f, 0.0f, radius);
	display_gl_generate_sphere(sub_division, &center, radius * 0.5f, vb + vb_size * 5, def, ib + ib_count * 5, vb_count * 5);

	display->process_mesh = display_gl_mesh_vn_create(vb, vb_count * count, ib, ib_count * count);

	free(vb);
	free(ib);
}

void display_gl_process_update(struct vm_s* vm, struct display_gl_s* display) {

}


void display_gl_process_render(struct vm_s* vm, struct display_gl_s* display) {
	v4_t		color_io_process;
	v4_t		color_ambient;
	v3_t		light_direction;
	mat4_t	local;
	mat4_t	translate;
	mat4_t	rotation;
	quat_t	quat;
	int			i;
	uint8*	process_counter;

	v4_t 		selected_process_color[2] = {
		{1.f, 1.f, 1.f, 1.f},
		{1.f, 0.f, 0.f, 0.f},
	};

	process_counter = (uint8*)display->io_read_buffer;

	v4_set(&color_io_process, 0.4f, 0.4f, 1.0f, 0.0f);
	v4_set(&color_ambient, 0.2f, 0.2f, 0.2f, 1.0f);
	v3_set(&light_direction, 0, 0, 1.0f);


	display_gl_mesh_render_start(display->mesh_renderer, MESH_TYPE_VN);
	display_gl_mesh_set_ambient(display->mesh_renderer, &color_ambient);
	display_gl_mesh_set_light_direction(display->mesh_renderer, &light_direction);
	display_gl_mesh_set_diffuse(display->mesh_renderer, &color_io_process);
	display_gl_mesh_set_projection(display->mesh_renderer, &display->projection_view);
	mat4_ident(&local);

	memset(process_counter, 0, display->memory_size);

	for (i = 0; i < vm->process_count; ++i)
	{
		process_t* process = vm->processes[i];
		int index = process->pc;

		if (process_counter[index] < DISPLAY_MAX_PROCES_PER_ADDRESS || (process == vm->step_process) ) {
			mat4_t normal;
			v4_t*	 color = (v4_t*)process->core->color;
			float x, y, z;
			
			z = DISPLAY_CELL_SIZE * 0.5f;
			if (process == vm->step_process && vm->step != -1) {
				int current = (int) (display->total_time) % 2;
				color = &selected_process_color[current];
				z +=  DISPLAY_CELL_SIZE;
			}

			process->angle += (float)display->frame_delta;

			x = (float)(index % display->memory_stride);
			y = (float)(index / display->memory_stride);

			process_counter[index]++;

			x = x * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;
			y = y * DISPLAY_CELL_SIZE + DISPLAY_CELL_SIZE * 0.5f;

			mat4_ident(&translate);
			mat4_translate(&translate, x, y, z);

			quat_from_euler(&quat, process->angle, process->angle, process->angle);
			quat_to_mat4(&quat, &rotation);
			mat4_invert(&rotation, &normal);
			mat4_transpose(&normal, &normal);
			mat4_mul(&translate, &rotation, &local);

			display_gl_mesh_set_diffuse(display->mesh_renderer, color);
			display_gl_mesh_set_local(display->mesh_renderer, &local);
			display_gl_mesh_set_normal(display->mesh_renderer, &normal);

			display_gl_mesh_render(display->process_mesh);
		}
	}
}


void display_gl_process_destroy(struct display_gl_s* display) {
	display_gl_mesh_destroy(display->process_mesh);
}
