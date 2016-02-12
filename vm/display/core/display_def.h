#ifndef __DIPLAY_DEF_H__
#define __DIPLAY_DEF_H__



#define DATA_PATH ".\\data\\"
#define SHADER(name) DATA_PATH ## "shaders\\" ## name
#define TEXTURE(name) DATA_PATH ## "textures\\" ## name

#include "display_gl.h"
#include "display_math.h"
#include "display_mesh.h"
#include "display_text.h"


#define DISPLAY_CELL_SIZE				10.f
#define DISPLAY_OK						1

typedef struct display_s
{
	GLFWwindow*	window;


	GLuint			hex_texture;
	GLuint			write_texture;
	GLuint			read_texture;
	GLuint			live_texture;

	shader_t		memory_shader;
	GLuint			grid_vertex_buffer;
	GLuint			grid_index_buffer;
	GLuint			memory_vertex_buffer;

	int32				grid_width;
	int32				grid_height;

	uint8*			memory_read_buffer;
	uint8*			memory_write_buffer;

	GLuint			memory_uniform_projection_matrix;
	GLuint			memory_uniform_coord;
	GLuint			memory_uniform_color;
	GLuint			memory_uniform_texture;
	GLuint			memory_vao;
	int32				memory_vertex_count;
	int32				memory_index_count;
	int32				memory_size;
	float				memory_width;
	float				memory_height;
	int32				memory_stride;

	shader_t		io_shader;
	GLuint			io_uniform_projection_matrix;
	GLuint			io_uniform_color;
	GLuint			io_uniform_texture;

	int32			frame_buffer_width;
	int32			frame_buffer_height;
	float			frame_buffer_ratio;

	double		mouse_prev_x;
	double		mouse_prev_y;

	float			display_zoom;
	float			display_center_x;
	float			display_center_y;

	double		frame_last_time;
	double		frame_delta;

	t_mesh*										live_mesh;
	uint32										live_count;
	int8*											live_vertex_buffer;

	t_display_mesh_renderer*	mesh_renderer;
	t_mesh*										process_mesh;

	t_mesh*										jump_mesh;
	int8*											jump_vertex_buffer;
	uint32										jump_count;

	mat4_t										projection_view;
	display_text_t*						texts;

} display_t;

#endif
