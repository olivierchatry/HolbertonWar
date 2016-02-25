#include <memory.h>
#include <stb_image.h>
#include <math.h>
#include <stdlib.h>

#include	"../../common/holberton-core.h"
#include	"../../common/ring_buffer.h"

#include "../../vm.h"
#include "../../core.h"

#include "core/display_gl_def.h"

#include "display_gl_grid.h"
#include "display_gl_memory.h"
#include "display_gl_io.h"
#include "display_gl_live.h"
#include "display_gl_jump.h"
#include "display_gl_process.h"
#include "display_gl_sky.h"

float	display_gl_text(display_gl_t* display, float x, float y, int32 rgba, char* format, ...) {
	va_list args;
	float ret;

	va_start(args, format);
	ret = display_gl_text_add_va(display->texts, x, y, rgba, format, args);
	va_end(args);
	return ret;
}

void display_gl_scroll_callback(GLFWwindow* window, double dx, double dy) {
	display_gl_t* display = glfwGetWindowUserPointer(window);
	float display_gl_zoom = display->display_gl_zoom - (float) dy / 10.0f;

	if (display_gl_zoom < 0.01f)
		display_gl_zoom = 0.01f;
	float width = display->frame_buffer_width  * display->display_gl_zoom;
	float height = display->frame_buffer_height * display->display_gl_zoom;

	float newWidth = display->frame_buffer_width  * display_gl_zoom;
	float newHeight = display->frame_buffer_height * display_gl_zoom;

	double mouse_cur_x, mouse_cur_y;
	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);

	float x = ((float)mouse_cur_x / display->frame_buffer_width) - 0.5f;
	float y = ((float)mouse_cur_y / display->frame_buffer_height) - 0.5f;

	float tx = (newWidth - width) * -x;
	float ty = (newHeight - height) * -y;


	display->display_gl_zoom = display_gl_zoom;

	display->display_gl_center_x += tx;
	display->display_gl_center_y += ty;
}

display_gl_t* display_gl_initialize(int width, int height, int full_screen) {
	display_gl_t*			display;

	if (!glfwInit()) {
		return NULL;
	}
	display = (display_gl_t*)malloc(sizeof(display_gl_t));
	memset(display, 0, sizeof(display_gl_t));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
	printf("running on OSX, set compat flag\n");
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	int widthMM, heightMM;
	glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	const float dpi = (float) mode->width / ((float) widthMM / 25.4f);

	display->dpi_text_scale = dpi / 96.0f;

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
	glfwWindowHint(GLFW_FLOATING, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif
	display->screen_ratio = (float) mode->width / (float) mode->height;
	if (full_screen) {
		display->window = glfwCreateWindow(mode->width, mode->height, "HolbertonWar", monitor, NULL);
	}
	else {
		display->window = glfwCreateWindow(width, height, "HolbertonWar", NULL, NULL);
	}

	glfwMakeContextCurrent(display->window);
	glewExperimental = GL_TRUE;
	glewInit();

	display->mesh_renderer = display_gl_mesh_renderer_initialize();

	display_gl_grid_init(display, VM_MEMORY_SIZE);
	display_gl_memory_init(display);
	display_gl_io_init(display);
	display_gl_live_init(display);
	display_gl_process_init(display);
	display_gl_jump_init(display);
	display_gl_sky_init(display);

	display->texts = display_gl_text_intialize();


	glfwGetCursorPos(display->window, &display->mouse_prev_x, &display->mouse_prev_y);
	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);

	float screen_memory_ratio = display->memory_width / (float) display->frame_buffer_width;
	float screen_memory_ratio_height = display->memory_height / (float) display->frame_buffer_height;
	if (screen_memory_ratio_height > screen_memory_ratio)
		screen_memory_ratio = screen_memory_ratio_height;

	display->display_gl_zoom = screen_memory_ratio;
	display->display_gl_center_x = ((float)display->memory_width * 0.5f);
	display->display_gl_center_y = ((float)display->memory_height * 0.5f);

	display->frame_last_time = glfwGetTime();
	glfwSetWindowUserPointer(display->window, (void*)display);
	glfwSetScrollCallback(display->window, display_gl_scroll_callback);
	// glfwSetCursorPosCallback(display->window, display_gl_mouse_move_callback);
	glfwSwapInterval(0);
	return display;
}

int	 display_gl_should_exit(display_gl_t* display) {
	return display && glfwWindowShouldClose(display->window);
}

void display_gl_destroy(display_gl_t* display) {
	display_gl_memory_destroy(display);
	display_gl_io_destroy(display);
	display_gl_live_destroy(display);
	display_gl_jump_destroy(display);
	display_gl_process_destroy(display);
	display_gl_grid_destroy(display);
	display_gl_sky_destroy(display);
	display_gl_text_destroy(display->texts);

	display_gl_mesh_renderer_destroy(display->mesh_renderer);

	glfwDestroyWindow(display->window);
	free(display);
	glfwTerminate();
}

int32 display_gl_update_input(display_gl_t* display) {
	int32 moved = 0;
	double mouse_cur_x, mouse_cur_y;
	double mouse_delta_x, mouse_delta_y;

	double 	current_time = glfwGetTime();
	double 	delta = current_time - display->frame_last_time;
	float		display_gl_zoom = display->display_gl_zoom;

	display->frame_delta = delta;
	display->total_time += delta;
	display->frame_last_time = current_time;

	glfwGetCursorPos(display->window, &mouse_cur_x, &mouse_cur_y);

	mouse_delta_x = display->mouse_prev_x - mouse_cur_x;
	mouse_delta_y = display->mouse_prev_y - mouse_cur_y;

	display->mouse_prev_x = mouse_cur_x;
	display->mouse_prev_y = mouse_cur_y;


	if (glfwGetKey(display->window, GLFW_KEY_Q) == GLFW_PRESS) {
		display_gl_zoom += (float)delta, moved = 1;
	}

	if (glfwGetKey(display->window, GLFW_KEY_A) == GLFW_PRESS) {
		display_gl_zoom -= (float)delta, moved = 1;
	}

	if (glfwGetKey(display->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(display->window, GL_TRUE);
	}


	if (display_gl_zoom < 0.01f) {
		display_gl_zoom = 0.01f;
	}

	if (moved) {

		float width   = display->frame_buffer_width  * display->display_gl_zoom;
		float height  = display->frame_buffer_height * display->display_gl_zoom;

		float newWidth = display->frame_buffer_width  * display_gl_zoom;
		float newHeight = display->frame_buffer_height * display_gl_zoom;


		float x = ((float)mouse_cur_x / display->frame_buffer_width) - 0.5f;
		float y = ((float)mouse_cur_y / display->frame_buffer_height) - 0.5f;

		float tx = (newWidth - width) * -x;
		float ty = (newHeight - height) * -y;


		display->display_gl_zoom = display_gl_zoom;


		display->display_gl_center_x += tx;
		display->display_gl_center_y += ty;
	}


	if (glfwGetMouseButton(display->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		display->display_gl_center_x += (float)mouse_delta_x * display->display_gl_zoom;
		display->display_gl_center_y += (float)mouse_delta_y * display->display_gl_zoom;
		moved = 1;
	}
	return moved;
}

void display_gl_camera_update(display_gl_t* display) {
	float width = display->frame_buffer_width  * display->display_gl_zoom * 0.5f;
	float height = display->frame_buffer_height * display->display_gl_zoom * 0.5f;

	glfwGetFramebufferSize(display->window, &display->frame_buffer_width, &display->frame_buffer_height);
	display->frame_buffer_ratio =
		(float)display->frame_buffer_width / (float)display->frame_buffer_height;
	glViewport(0, 0, display->frame_buffer_width, display->frame_buffer_height);

	mat4_ident(&display->projection_view);

	mat4_ortho(&display->projection_view,
		display->display_gl_center_x - width,
		display->display_gl_center_x + width,
		display->display_gl_center_y + height,
		display->display_gl_center_y - height,
		-10000.0f, 10000.0f);
}

void display_gl_texts(struct vm_s* vm, display_gl_t* display) {
	mat4_t projection;
	mat4_ident(&projection);
	const float scale = 1.0f / display->dpi_text_scale;

	mat4_ortho(&projection, 0, display->frame_buffer_width * scale, display->frame_buffer_height * scale, 0, -1000, 1000);
	display_gl_text_render(display->texts, &projection);
	display_gl_text_clear(display->texts);
}

void*	display_gl_get_window(display_gl_t* display) {
	return display->window;
}

void display_gl_step(struct vm_s* vm, display_gl_t* display) {
	glfwMakeContextCurrent(display->window);
	display_gl_camera_update(display);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	display_gl_sky_render(vm, display);


	display_gl_memory_update(vm, display);
	display_gl_memory_render(vm, display);

	display_gl_io_update(vm, display);
	display_gl_io_render(vm, display);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	display_gl_process_render(vm, display);

	glEnable(GL_BLEND);
	display_gl_jump_update(vm, display);
	display_gl_jump_render(vm, display);

	display_gl_live_update(vm, display);
	display_gl_live_render(vm, display);

	display_gl_texts(vm, display);
}

void display_gl_swap(display_gl_t* display) {
	glfwSwapBuffers(display->window);
	glfwPollEvents();
	display->frame_delta = 0;
}
