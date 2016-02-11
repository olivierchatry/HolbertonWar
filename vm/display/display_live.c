#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "display_def.h"
#include "display_memory.h"
#include "display_grid.h"

#include "display_mesh_generator.h"
#include "../vm.h"


void display_live_init(struct display_s* display) {
	int32 vertices_count;

	display->live_texture = display_gl_load_texture("data/live.png");

	display_generate_rect_count(&vertices_count);
	display->live_mesh = display_mesh_vtc_create(NULL, VM_MAX_PROCESSES * vertices_count, NULL, 0);
	display->live_vertex_buffer = malloc(VM_MAX_PROCESSES * vertices_count * display_mesh_get_definiton(MESH_TYPE_VTC)->stride);
}

void display_live_update(struct vm_s* vm, struct display_s* display) {
	int32		i;
	int8*	live_vertex_buffer = display->live_vertex_buffer;
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VTC);

	display->live_count = 0;
	for (i = 0; i < vm->process_count; ++i)
	{
		process_t* process = vm->processes[i];

		if (process->current_opcode && process->current_opcode->opcode == 0x0c) {
			uint32 live_color = process->last_core_live->color_uint /*& 0xffffff*/;
			v3_t	start, end;
			float size = LERP(0.0f, DISPLAY_CELL_SIZE, (float)(process->current_opcode->cycles - process->cycle_wait) / (float)process->current_opcode->cycles);

			//live_color |= 0x10000000;
			display_grid_get_position(display, process->pc, &start);

			end.x = start.x + size;
			end.y = start.y + size;
			end.z = start.z;
			start.x -= size;
			start.y -= size;

			live_vertex_buffer = display_generate_rect(&start, &end, live_color, live_vertex_buffer, def);
			display->live_count++;
		}
	}

}

void display_live_render(struct vm_s* vm, struct display_s* display) {
	v4_t								color_diffuse;
	v4_t								color_ambient;
	mat4_t							local;
	t_mesh_definition* 	def = display_mesh_get_definiton(MESH_TYPE_VTC);
	int32 						 	vertices_count;

	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	display_generate_rect_count(&vertices_count);
	if (display->live_count > 0) {
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, display->live_texture);
		glBindBuffer(GL_ARRAY_BUFFER, display_mesh_get_vb(display->live_mesh));
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->live_count * def->stride * vertices_count, display->live_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		display_mesh_render_start(display->mesh_renderer, MESH_TYPE_VTC);
		display_mesh_set_ambient(display->mesh_renderer, &color_ambient);
		display_mesh_set_diffuse(display->mesh_renderer, &color_diffuse);
		display_mesh_set_local(display->mesh_renderer, &local);
		display_mesh_set_projection(display->mesh_renderer, &display->projection_view);
		display_mesh_render_count(display->live_mesh, display->live_count * vertices_count);
	}
}

void display_live_destroy(struct display_s* display) {
	display_mesh_destroy(display->live_mesh);
	glDeleteTextures(1, &display->live_texture);
	free(display->live_vertex_buffer);
}
