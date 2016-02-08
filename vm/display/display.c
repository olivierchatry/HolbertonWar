#include <GL/glew.h>
#if defined(_WIN32)
#	include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>
#include <memory.h>
#include <stb_image.h>
#include <math.h>
#include <stdlib.h>

#include	"../../common/holberton-core.h"
#include	"../../common/ring_buffer.h"

#include "../vm.h"
#include "../core.h"

#include "display.h"
#include "display_math.h"
#include "display_gl.h"
#include "display_mesh.h"
#include "display_mesh_generator.h"
#include "display_text.h"

typedef struct display_s
{
	GLFWwindow*	window;


	GLuint			hex_texture;
	GLuint			write_texture;
	GLuint			read_texture;
	shader_t		memory_shader;
	GLuint			memory_grid_vertex_buffer;
	GLuint			memory_grid_index_buffer;
	GLuint			memory_vertex_buffer;
	
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

	display_mesh_renderer_t*	mesh_renderer;
	mesh_t*										process_mesh;

	mesh_t*										jump_mesh;
	int8*											jump_vertex_buffer;
	uint32										jump_count;

	mat4_t										projection_view;
	display_text_t*						texts;

} display_t;



typedef struct s_grid_vertex
{
	v3_t	v;
	float	i;
}	t_grid_vertex;

static int32 g_grid_width;

void display_get_position(int addr, v3_t* v) {
	int32 x = addr % g_grid_width;
	int32 y = (addr - x) / g_grid_width;

	v->z = 0;
	v->x = DISPLAY_CELL_SIZE * x + DISPLAY_CELL_SIZE * 0.5f;
	v->y = DISPLAY_CELL_SIZE * y + DISPLAY_CELL_SIZE * 0.5f;
}

void display_generate_grid(display_t* display, int memory_size)
{
	g_grid_width = (int32)roundf(sqrtf(VM_MEMORY_SIZE));
	int32						width = g_grid_width;
	int32						height = width;
	int32						size = width * height;
	int32						x, y;
	t_grid_vertex*	vb;
	t_grid_vertex*	temp_vb;
	uint16*					ib;
	uint16*					temp_ib;
	int32						vb_size = (size + height) * 4 * sizeof(t_grid_vertex);
	int32						ib_size = (size + height) * 6 * sizeof(int16);
	int32						cell_index = 0;

	temp_vb = vb = malloc(vb_size);
	temp_ib = ib = malloc(ib_size);
	for (y = 0; y < height + 1; ++y)
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
	display->memory_grid_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, vb_size, GL_STATIC_DRAW, temp_vb);
	display->memory_grid_index_buffer = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, ib_size, GL_STATIC_DRAW, temp_ib);
	display->memory_vertex_buffer = display_gl_create_buffer(GL_ARRAY_BUFFER, (size + height) * 4, GL_STREAM_DRAW, NULL);
	display->memory_read_buffer = (uint8*)malloc((size + height) * 4);
	display->memory_write_buffer = (uint8*)malloc((size + height) * 4);
	display->memory_vertex_count = (size + height) * 4;
	display->memory_index_count = (size + height) * 6;
	display->memory_size = (size + height);
	display->memory_width = DISPLAY_CELL_SIZE * width;
	display->memory_height = DISPLAY_CELL_SIZE * height + 1;
	display->memory_stride = width;
	free(temp_vb);
	free(temp_ib);

}
void		display_generate_process_mesh(display_t* display)
{
	int32		vb_count;
	int32		ib_count;
	uint8*	vb;
	uint16* ib;
	v3_t 		center;
	float 	radius = DISPLAY_CELL_SIZE * 0.25f;
	mesh_definition_t* def;
	int32 vb_size;
	int32 count = 6;
	int32 sub_division = 16;

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
	display->jump_mesh = display_mesh_vc_create(NULL, VM_MAX_PROCESSES * 6, NULL, 0);
	display->jump_vertex_buffer = malloc(VM_MAX_PROCESSES * 6 * display_mesh_get_definiton(MESH_TYPE_VC)->stride);
	free(vb);
	free(ib);
}

float	display_text(display_t* display, float x, float y, int32 rgba, char* format, ...) {
	va_list args;
	float ret;

	va_start(args, format);
	ret = display_text_add_va(display->texts, x, y, rgba, format, args);
	va_end(args);
	return ret;
}

void display_scroll_callback(GLFWwindow* window, double dx, double dy)
{
	display_t* display = glfwGetWindowUserPointer(window);
	float display_zoom = display->display_zoom - (float) dy / 10.0f;

	if (display_zoom < 0.01f)
		display_zoom = 0.01f;
	float width = display->frame_buffer_width  * display->display_zoom;
	float height = display->frame_buffer_height * display->display_zoom;

	float newWidth = display->frame_buffer_width  * display_zoom;
	float newHeight = display->frame_buffer_height * display_zoom;

	double mouse_cur_x, mouse_cur_y;
	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);

	float x = ((float)mouse_cur_x / display->frame_buffer_width) - 0.5f;
	float y = ((float)mouse_cur_y / display->frame_buffer_height) - 0.5f;

	float tx = (newWidth - width) * -x;
	float ty = (newHeight - height) * -y;


	display->display_zoom = display_zoom;


	display->display_center_x += tx;
	display->display_center_y += ty;
}

display_t* display_initialize(int width, int height, int full_screen)
{
	display_t*			display = (display_t*)malloc(sizeof(display_t));
	location_t			location[] = {
		{"in_Position", 0},
		{"in_Value", 1},
		{NULL, 0}
	};

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
	printf("running on OSX, set compat flag\n");
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
	glfwWindowHint(GLFW_FLOATING, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif

	if (full_screen) {
		display->window = glfwCreateWindow(mode->width, mode->height, "HolbertonWar", monitor, NULL);
	}
	else {
		display->window = glfwCreateWindow(width, height, "HolbertonWar", NULL, NULL);
	}

	glfwMakeContextCurrent(display->window);
	glewExperimental = GL_TRUE;
	glewInit();

	display_generate_grid(display, VM_MEMORY_SIZE);
	display->hex_texture = display_gl_load_texture("data/hex.png");
	display->write_texture = display_gl_load_texture("data/write.png");
	display->read_texture = display_gl_load_texture("data/read.png");

	display_gl_load_shader(&display->memory_shader, "shaders/memory.vert", "shaders/memory.frag", location);
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

	display_gl_load_shader(&display->io_shader, "shaders/io.vert", "shaders/io.frag", location);
	display->io_uniform_projection_matrix = glGetUniformLocation(display->io_shader.id, "uni_ProjectionMatrix");
	display->io_uniform_color = glGetUniformLocation(display->io_shader.id, "uni_Color");
	display->io_uniform_texture = glGetUniformLocation(display->io_shader.id, "uni_Texture");

	display->memory_vao = display_gl_create_vao();
	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_grid_vertex_buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, 1, (const void*)0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// glDisable(GL_CULL_FACE);
	// glDisable(GL_DEPTH_TEST);

	display->mesh_renderer = display_mesh_renderer_initialize();

	glfwGetCursorPos(display->window, &display->mouse_prev_x, &display->mouse_prev_y);
	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);

	float screen_memory_ratio = display->memory_width / (float) display->frame_buffer_width;
	float screen_memory_ratio_height = display->memory_height / (float) display->frame_buffer_height;
	if (screen_memory_ratio_height > screen_memory_ratio)
		screen_memory_ratio = screen_memory_ratio_height;

	display->display_zoom = screen_memory_ratio;
	display->display_center_x = ((float)display->frame_buffer_width * 0.5f) * screen_memory_ratio;
	display->display_center_y = ((float)display->frame_buffer_height * 0.5f) * screen_memory_ratio;

	display->frame_last_time = glfwGetTime();
	display_generate_process_mesh(display);
	display->texts = display_text_intialize();
	glfwSetWindowUserPointer(display->window, (void*)display);
	glfwSetScrollCallback(display->window, display_scroll_callback);
	// glfwSetCursorPosCallback(display->window, display_mouse_move_callback);
	glfwSwapInterval(0);
	return display;
}

int	 display_should_exit(display_t* display)
{
	return glfwWindowShouldClose(display->window);
}

void display_destroy(display_t* display) {
	display_mesh_destroy(display->process_mesh);
	display_mesh_destroy(display->jump_mesh);

	display_mesh_renderer_destroy(display->mesh_renderer);
	glDeleteBuffers(1, &display->memory_vertex_buffer);
	glDeleteBuffers(1, &display->memory_grid_vertex_buffer);
	glDeleteBuffers(1, &display->memory_grid_index_buffer);
	glDeleteTextures(1, &display->hex_texture);
	display_gl_destroy_shader(&display->memory_shader);
	display_gl_destroy_shader(&display->io_shader);
	glDeleteVertexArrays(1, &display->memory_vao);
	display_text_destroy(display->texts);

	glfwDestroyWindow(display->window);
	free(display->memory_read_buffer);
	free(display->memory_write_buffer);
	free(display->jump_vertex_buffer);
	glfwTerminate();
}

void display_update_memory(struct vm_s* vm, display_t* display)
{
	uint8* dst;
	uint8*	src = (uint8*)vm->memory;
	int		size = vm->memory_size;


	dst = (uint8*)display->memory_write_buffer;
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_write_buffer);
}

void display_render_memory(struct vm_s* vm, display_t* display)
{
	float color_mem[] = { 0.4f, 0.4f, 0.8f, 1.0f };

	display_update_memory(vm, display);
	glUseProgram(display->memory_shader.id);
	glUniformMatrix4fv(display->memory_uniform_projection_matrix, 1, GL_FALSE, display->projection_view.mat.v);
	glUniform4fv(display->memory_uniform_color, 1, color_mem);

	glBindTexture(GL_TEXTURE_2D, display->hex_texture);
	glBindVertexArray(display->memory_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, display->memory_grid_index_buffer);
	glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void display_update_jump(vm_t* vm, display_t* display) {
	
}

void display_render_io(struct vm_s* vm, display_t* display)
{
	int			i, j, c;
	uint8*  dst_write;
	uint8*	dst_read;
	int			size = vm->memory_size;
	mesh_definition_t* def = display_mesh_get_definiton(MESH_TYPE_VC);

	dst_write = (uint8*)display->memory_write_buffer;
	dst_read = (uint8*)display->memory_read_buffer;


	display->jump_count = 0;

	uint8* jump_vertex_buffer = display->jump_vertex_buffer;

	for (c = 1; c < vm->core_count; ++c)
	{
		core_t* core = vm->cores[c];
		uint32 jump_color = core->color_uint & 0xffffff;
		jump_color |= 0x10000000;

		memset(dst_read, 0, display->memory_size * 4);
		memset(dst_write, 0, display->memory_size * 4);


		for (i = 0; i < vm->process_count; ++i)
		{
			process_t* process = vm->processes[i];
			if (process->core == core)
			{
				for (j = 0; j < process->memory_read_op_count; ++j)
				{
					int index = process->memory_read_op[j] * 4;
					dst_read[index + 0]++;
					dst_read[index + 1]++;
					dst_read[index + 2]++;
					dst_read[index + 3]++;
				}

				for (j = 0; j < process->memory_write_op_count; ++j)
				{
					int index = process->memory_write_op[j] * 4;
					dst_write[index + 0]++;
					dst_write[index + 1]++;
					dst_write[index + 2]++;
					dst_write[index + 3]++;
				}

				if (process->jump) {
					v3_t start;
					v3_t end;
					float size = LERP(0.0f, DISPLAY_CELL_SIZE, (float) process->cycle_wait / (float) process->current_opcode->cycles);
					display_get_position(process->jump_from, &start);
					display_get_position(process->jump_to, &end);
					jump_vertex_buffer = display_generate_line(&start, &end, size, size * 0.5f, jump_color, jump_vertex_buffer, def);
					display->jump_count++;
				}
			}
		}


		glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_read_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, display->read_texture);
		glUseProgram(display->io_shader.id);

		glUniformMatrix4fv(display->io_uniform_projection_matrix, 1, GL_FALSE, display->projection_view.mat.v);
		glUniform4fv(display->io_uniform_color, 1, core->color);
		glBindVertexArray(display->memory_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, display->memory_grid_index_buffer);
		glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);	
		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, display->memory_vertex_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->memory_size * 4, display->memory_write_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(display->memory_vao);
		glBindTexture(GL_TEXTURE_2D, display->write_texture);
		glDrawElements(GL_TRIANGLES, display->memory_index_count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}


void display_render_io_process(struct vm_s* vm, display_t* display)
{
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

int32 display_update_input(display_t* display)
{
	int32 moved = 0;
	double mouse_cur_x, mouse_cur_y;
	double mouse_delta_x, mouse_delta_y;

	double current_time = glfwGetTime();
	double delta = display->frame_last_time - current_time;
	display->frame_delta = delta;
	double framebuffer_center_x = (double)display->frame_buffer_width * 0.5f;
	double framebuffer_center_y = (double)display->frame_buffer_height * 0.5f;
	float		display_zoom = display->display_zoom;
	display->frame_last_time = current_time;

	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);

	mouse_delta_x = display->mouse_prev_x - mouse_cur_x;
	mouse_delta_y = display->mouse_prev_y - mouse_cur_y;

	display->mouse_prev_x = mouse_cur_x;
	display->mouse_prev_y = mouse_cur_y;


	if (glfwGetKey(display->window, GLFW_KEY_Q) == GLFW_PRESS)
		display_zoom += (float)delta, moved = 1;

	if (glfwGetKey(display->window, GLFW_KEY_A) == GLFW_PRESS)
		display_zoom -= (float)delta, moved = 1;

	if (glfwGetKey(display->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(display->window, GL_TRUE);

	if (display_zoom < 0.01f)
		display_zoom = 0.01f;


	if (moved)
	{

		float width   = display->frame_buffer_width  * display->display_zoom;
		float height  = display->frame_buffer_height * display->display_zoom;

		float newWidth = display->frame_buffer_width  * display_zoom;
		float newHeight = display->frame_buffer_height * display_zoom;


		float x = ((float)mouse_cur_x / display->frame_buffer_width) - 0.5f;
		float y = ((float)mouse_cur_y / display->frame_buffer_height) - 0.5f;

		float tx = (newWidth - width) * -x;
		float ty = (newHeight - height) * -y;


		display->display_zoom = display_zoom;


		display->display_center_x += tx;
		display->display_center_y += ty;
	}


	if (glfwGetMouseButton(display->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		display->display_center_x += (float)mouse_delta_x * display->display_zoom;
		display->display_center_y += (float)mouse_delta_y * display->display_zoom;
		moved = 1;
	}
	return moved;
}

void display_update_camera(display_t* display)
{
	float width = display->frame_buffer_width  * display->display_zoom * 0.5f;
	float height = display->frame_buffer_height * display->display_zoom * 0.5f;

	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);
	display->frame_buffer_ratio =
		(float)display->frame_buffer_width / (float)display->frame_buffer_height;
	glViewport(0, 0, display->frame_buffer_width, display->frame_buffer_height);

	mat4_ident(&display->projection_view);

	mat4_ortho(&display->projection_view,
		display->display_center_x - width,
		display->display_center_x + width,
		display->display_center_y + height,
		display->display_center_y - height,
		-1000.0f, 1000.0f);
}

void display_render_jump(display_t* display) {
	v4_t	color_diffuse;
	v4_t	color_ambient;
	mat4_t	local;
	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	if (display->jump_count > 0) {
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindBuffer(GL_ARRAY_BUFFER, display_mesh_get_vb(display->jump_mesh));
		glBufferSubData(GL_ARRAY_BUFFER, 0, display->jump_count * 16 * 6, display->jump_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		display_mesh_render_start(display->mesh_renderer, MESH_TYPE_VC);
		display_mesh_set_ambient(display->mesh_renderer, &color_ambient);
		display_mesh_set_diffuse(display->mesh_renderer, &color_diffuse);
		display_mesh_set_local(display->mesh_renderer, &local);
		display_mesh_set_projection(display->mesh_renderer, &display->projection_view);
		display_mesh_render_count(display->jump_mesh, display->jump_count * 6);
	}
}

void display_step(struct vm_s* vm, display_t* display)
{
	display_update_camera(display);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	display_render_memory(vm, display);

	display_render_io(vm, display);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	display_render_io_process(vm, display);
	glEnable(GL_BLEND);
	{
		mat4_t projection;
		mat4_ident(&projection);
		mat4_ortho(&projection, 0, display->frame_buffer_width * 0.5f, display->frame_buffer_height * 0.5f, 0, -1000, 1000);
		display_text_render(display->texts, &projection);
		display_text_clear(display->texts);
	}

	display_render_jump(display);
	glfwSwapBuffers(display->window);
	glfwPollEvents();

}
