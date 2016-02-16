#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "core/display_gl_def.h"
#include "core/display_gl_mesh_generator.h"

#include "display_gl_memory.h"
#include "display_gl_grid.h"

#include "../../vm.h"


void display_gl_jump_init(struct display_gl_s* display) {
	int32 vertices_count;
	display_gl_generate_arrow_count(&vertices_count);
	display->jump_mesh = display_gl_mesh_vc_create(NULL, VM_MAX_PROCESSES * vertices_count, NULL, 0);
	display->jump_vertex_buffer = malloc(VM_MAX_PROCESSES * vertices_count * display_gl_mesh_get_definiton(MESH_TYPE_VC)->stride);
}

void display_gl_jump_update(struct vm_s* vm, struct display_gl_s* display) {
	int32		i;
	int8*	jump_vertex_buffer = display->jump_vertex_buffer;
	mesh_definition_t* def = display_gl_mesh_get_definiton(MESH_TYPE_VC);

	display->jump_count = 0;

	for (i = 0; i < vm->process_count; ++i) {
		process_t* process = vm->processes[i];
		if (process->current_opcode &&
				(process->current_opcode->opcode == 7 || process->current_opcode->opcode == 6) ) {
			uint32 jump_color = process->core->color_uint & 0xffffff;
			v3_t	start, end;
			// float size = LERP(0.0f, DISPLAY_CELL_SIZE, (float)(process->cycle_wait) / (float)process->current_opcode->cycles);
			float size = DISPLAY_CELL_SIZE;
			jump_color |= 0x10000000;
			display_gl_grid_get_position(display, process->jump_from, &start);
			display_gl_grid_get_position(display, process->jump_to, &end);
			jump_vertex_buffer = display_gl_generate_arrow(&start, &end, size, jump_color, jump_vertex_buffer, def);
			display->jump_count++;
		}

	}

}

void display_gl_jump_render(struct vm_s* vm, struct display_gl_s* display) {
	v4_t	color_diffuse;
	v4_t	color_ambient;
	mat4_t	local;
	mesh_definition_t* 	def = display_gl_mesh_get_definiton(MESH_TYPE_VC);
	int32 						 	vertices_count;


	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	display_gl_generate_line_count(&vertices_count);
	if (display->jump_count > 0) {
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindBuffer(GL_ARRAY_BUFFER, display_gl_mesh_get_vb(display->jump_mesh));
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->jump_count * def->stride * vertices_count, display->jump_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		display_gl_mesh_render_start(display->mesh_renderer, MESH_TYPE_VC);
		display_gl_mesh_set_ambient(display->mesh_renderer, &color_ambient);
		display_gl_mesh_set_diffuse(display->mesh_renderer, &color_diffuse);
		display_gl_mesh_set_local(display->mesh_renderer, &local);
		display_gl_mesh_set_projection(display->mesh_renderer, &display->projection_view);
		display_gl_mesh_render_count(display->jump_mesh, display->jump_count * vertices_count);
	}
}

void display_gl_jump_destroy(struct display_gl_s* display) {
	display_gl_mesh_destroy(display->jump_mesh);
	free(display->jump_vertex_buffer);
}
