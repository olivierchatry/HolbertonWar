#ifndef __DIPLAY_DEF_H__
#define __DIPLAY_DEF_H__


#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#define DATA_PATH "data" PATH_SEP

#define SHADER(name) DATA_PATH  "shaders" PATH_SEP name
#define TEXTURE(name) DATA_PATH  "textures" PATH_SEP name

#include "display_gl_helpers.h"
#include "display_gl_math.h"
#include "display_gl_mesh.h"
#include "display_gl_text.h"


#define DISPLAY_CELL_SIZE				10.f
#define DISPLAY_OK						1

#define DISPLAY_MAX_LIVE_PER_ADDRESS		8
#define DISPLAY_MAX_PROCES_PER_ADDRESS	8
#define DISPLAY_MAX_JUMP_PER_CORE				100

typedef struct display_gl_s
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

	uint8*			io_read_buffer;
	uint8*			io_write_buffer;

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
	float			screen_ratio;

	double		mouse_prev_x;
	double		mouse_prev_y;

	float			display_gl_zoom;
	float			display_gl_center_x;
	float			display_gl_center_y;

	double		frame_last_time;
	double		frame_delta;

	display_gl_mesh_renderer_t*	mesh_renderer;

	mesh_t*										live_mesh;
	uint32										live_count;
	int8*											live_vertex_buffer;

	mesh_t*										process_mesh;

	mesh_t*										jump_mesh;
	int8*											jump_vertex_buffer;
	uint32										jump_count;

	mat4_t										projection_view;
	display_gl_text_t*				texts;

	float											dpi_text_scale;

	mesh_t*										sky_box;
	GLuint										sky_box_texture;
	float											sky_box_angle;
} display_gl_t;

#endif
