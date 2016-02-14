#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core/display_gl_def.h"
#include "display_gl_memory.h"
#include "display_gl_grid.h"

#include "core/display_gl_mesh_generator.h"
#include "../../vm.h"


void display_gl_live_init(struct display_gl_s* display) {
	int32 vertices_count;

	display->live_texture = display_gl_load_texture(TEXTURE("live.png"));

	display_gl_generate_rect_count(&vertices_count);
	display->live_mesh = display_gl_mesh_vtc_create(NULL, VM_MAX_PROCESSES * vertices_count, NULL, 0);
	display->live_vertex_buffer = malloc(VM_MAX_PROCESSES * vertices_count * display_gl_mesh_get_definiton(MESH_TYPE_VTC)->stride);
}

void display_gl_live_update(struct vm_s* vm, struct display_gl_s* display) {
	int32		i;
	int8*	live_vertex_buffer = display->live_vertex_buffer;
	mesh_definition_t* def = display_gl_mesh_get_definiton(MESH_TYPE_VTC);

	memset(vm->shadow, 0, VM_MEMORY_SIZE);

	display->live_count = 0;
	for (i = 0; i < vm->process_count; ++i)
	{
		process_t* process = vm->processes[i];
		int32 address = process->pc;
		if ( (vm->shadow[address] < DISPLAY_MAX_LIVE_PER_ADDRESS) 
			&& process->current_opcode 
			&& (process->current_opcode->opcode == 0x0c)) {
			uint32 live_color = process->last_core_live->color_uint & 0xffffff;
			v3_t	start, end;
			float size = LERP(0.0f, DISPLAY_CELL_SIZE * 2.0f, (float)(process->cycle_wait) / (float)process->current_opcode->cycles);

			vm->shadow[address]++;
			live_color |= 0x60000000;
			display_gl_grid_get_position(display, process->pc, &start);

			end.x = start.x + size;
			end.y = start.y + size;
			end.z = start.z;
			start.x -= size;
			start.y -= size;

			live_vertex_buffer = display_gl_generate_rect(&start, &end, live_color, live_vertex_buffer, def);
			display->live_count++;
		}
	}

}

void display_gl_live_render(struct vm_s* vm, struct display_gl_s* display) {
	v4_t								color_diffuse;
	v4_t								color_ambient;
	mat4_t							local;
	mesh_definition_t* 	def = display_gl_mesh_get_definiton(MESH_TYPE_VTC);
	int32 						 	vertices_count;

	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	display_gl_generate_rect_count(&vertices_count);
	if (display->live_count > 0) {
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, display->live_texture);
		glBindBuffer(GL_ARRAY_BUFFER, display_gl_mesh_get_vb(display->live_mesh));
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->live_count * def->stride * vertices_count, display->live_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		display_gl_mesh_render_start(display->mesh_renderer, MESH_TYPE_VTC);
		display_gl_mesh_set_ambient(display->mesh_renderer, &color_ambient);
		display_gl_mesh_set_diffuse(display->mesh_renderer, &color_diffuse);
		display_gl_mesh_set_local(display->mesh_renderer, &local);
		display_gl_mesh_set_projection(display->mesh_renderer, &display->projection_view);
		display_gl_mesh_render_count(display->live_mesh, display->live_count * vertices_count);
	}
}

void display_gl_live_destroy(struct display_gl_s* display) {
	display_gl_mesh_destroy(display->live_mesh);
	glDeleteTextures(1, &display->live_texture);
	free(display->live_vertex_buffer);
}
