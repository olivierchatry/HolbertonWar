#include <stdlib.h>
#include <string.h>

#include "core/display_gl_def.h"
#include "display_gl_memory.h"
#include "display_gl_grid.h"
#include "../../vm.h"


void display_gl_memory_init(struct display_gl_s* display) {
	location_t			location[] = {
		{"in_Position", 0},
		{"in_Value", 1},
		{NULL, 0}
	};
	int32 width = display->grid_width;
	int32 height = display->grid_height;
	int32 size = width * height;

	display->hex_texture = display_gl_load_texture(TEXTURE("hex.png"));

	display_gl_load_shader(&display->memory_shader, SHADER("memory.vert"), SHADER("memory.frag"), location);
	display->memory_uniform_projection_matrix = glGetUniformLocation(display->memory_shader.id, "uni_ProjectionMatrix");
	display->memory_uniform_coord = glGetUniformLocation(display->memory_shader.id, "uni_Coord");
	display->memory_uniform_color = glGetUniformLocation(display->memory_shader.id, "uni_Color");
	display->memory_uniform_texture = glGetUniformLocation(display->memory_shader.id, "uni_Texture");

	{
		float delta = 1.f / 255.f;
		float uv[] = {
			0.f, 0.f, delta, 0.f, 0.f, 1.0f, delta, 1.0f
		};
		int id = 0;
		glUseProgram(display->memory_shader.id);
		glUniform2fv(display->memory_uniform_coord, 4, uv);
		glUniform1iv(display->memory_uniform_texture, 1, &id);
		glUseProgram(0);
	}

	display->memory_vao = display_gl_create_vao();


	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 4, GL_STREAM_DRAW, NULL);
	display->memory_vertex_count = (size + height) * 4;
	display->memory_index_count = (size + height) * 6;
	display->memory_size = (size + height);
	display->memory_width = DISPLAY_CELL_SIZE * width;
	display->memory_height = DISPLAY_CELL_SIZE * height + 1;
	display->memory_stride = width;

	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ARRAY_BUFFER, display->grid_vertex_buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 1, (const void*)0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void display_gl_memory_destroy(struct display_gl_s* display) {
	glDeleteBuffers(1, &display->memory_vertex_buffer);
	glDeleteTextures(1, &display->hex_texture);
	glDeleteVertexArrays(1, &display->memory_vao);
	display_gl_destroy_shader(&display->memory_shader);
}

void display_gl_memory_update(struct vm_s* vm, display_gl_t* display) {
	uint8* dst;
	uint8*	src = (uint8*)vm->memory;
	int		size = vm->memory_size;


	dst = (uint8*)display->io_write_buffer;
	memset(dst, 0, display->memory_size * 4);

	while (size--)
	{
		uint8 v = *src++;

		*dst++ = v;
		*dst++ = v;
		*dst++ = v;
		*dst++ = v;
	}

	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->io_write_buffer);
}

void display_gl_memory_render(struct vm_s* vm, display_gl_t* display) {
	float color_mem[] = { 0.4f, 0.4f, 0.8f, 1.0f };

	glUseProgram(display->memory_shader.id);
	glUniformMatrix4fv(display->memory_uniform_projection_matrix, 1, GL_FALSE, display->projection_view.mat.v);
	glUniform4fv(display->memory_uniform_color, 1, color_mem);

	glBindTexture(GL_TEXTURE_2D, display->hex_texture);
	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, display->grid_index_buffer);
	glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
