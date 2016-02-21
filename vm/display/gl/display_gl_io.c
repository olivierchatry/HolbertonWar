
#include <stdlib.h>
#include <string.h>

#include "core/display_gl_def.h"

#include "display_gl_io.h"
#include "display_gl_grid.h"

#include "../../vm.h"

void display_gl_io_init(struct display_gl_s* display) {
	location_t			location[] = {
		{"in_Position", 0},
		{"in_Value", 1},
		{NULL, 0}
	};

	display->write_texture = display_gl_load_texture(TEXTURE("write.png"));
	display->read_texture = display_gl_load_texture(TEXTURE("read.png"));

	display_gl_load_shader(&display->io_shader, SHADER("io.vert"), SHADER("io.frag"), location);
	display->io_uniform_projection_matrix = glGetUniformLocation(display->io_shader.id, "uni_ProjectionMatrix");
	display->io_uniform_color = glGetUniformLocation(display->io_shader.id, "uni_Color");
	display->io_uniform_texture = glGetUniformLocation(display->io_shader.id, "uni_Texture");
	
	glUseProgram(display->io_shader.id);
	GLint		samplerId = 0;
	glUniform1iv(display->io_uniform_texture, 1, &samplerId);
	glUseProgram(0);

	display->io_read_buffer = (uint8*)malloc(display->memory_size * 4);
	display->io_write_buffer = (uint8*)malloc(display->memory_size * 4);
}

void display_gl_io_update(struct vm_s* vm, struct display_gl_s* display) {

}

void display_gl_io_render(struct vm_s* vm, struct display_gl_s* display) {
	int32		i, j, c;
	uint8*  dst_write;
	uint8*	dst_read;

	dst_write = (uint8*)display->io_write_buffer;
	dst_read = (uint8*)display->io_read_buffer;

	for (c = 1; c < vm->core_count; ++c) {
		core_t* core = vm->cores[c];

		memset(dst_read, 0, display->memory_size * 4);
		memset(dst_write, 0, display->memory_size * 4);

		for (i = 0; i < vm->process_count; ++i) {
			process_t* process = vm->processes[i];
			if (process->core == core) {
				for (j = 0; j < process->memory_read_op_count; ++j) {
					int index = process->memory_read_op[j] * 4;
					dst_read[index + 0]++;
					dst_read[index + 1]++;
					dst_read[index + 2]++;
					dst_read[index + 3]++;
				}

				for (j = 0; j < process->memory_write_op_count; ++j) {
					int index = process->memory_write_op[j] * 4;
					dst_write[index + 0]++;
					dst_write[index + 1]++;
					dst_write[index + 2]++;
					dst_write[index + 3]++;
				}
			}
		}


		glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->io_read_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, display->read_texture);
		glUseProgram(display->io_shader.id);

		glUniformMatrix4fv(display->io_uniform_projection_matrix, 1, GL_FALSE, display->projection_view.mat.v);
		glUniform4fv(display->io_uniform_color, 1, core->color);
		glBindVertexArray(display->memory_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, display->grid_index_buffer);
		glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->io_write_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(display->memory_vao);
		glBindTexture(GL_TEXTURE_2D, display->write_texture);
		glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void display_gl_io_destroy(struct display_gl_s* display) {
	glDeleteTextures(1, &display->read_texture);
	glDeleteTextures(1, &display->write_texture);
	display_gl_destroy_shader(&display->io_shader);
	free(display->io_read_buffer);
	free(display->io_write_buffer);
}
