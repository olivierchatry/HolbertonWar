#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core/display_gl_def.h"
#include "display_gl_memory.h"
#include "display_gl_grid.h"

#include "core/display_gl_mesh_generator.h"
#include "../../vm.h"

void display_gl_sky_init(struct display_gl_s* display) {
	int32								vertices_count;
	mesh_definition_t*	def;
	const float					size = 1.0f;
	v3_t								min = { -size, -size, -size};
	v3_t								max = { size, size, size };
	int8*								vb;

	display->sky_box_texture = display_gl_load_texture(TEXTURE("sky.png"));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	display_gl_generate_box_count(&vertices_count);
	def = display_gl_mesh_get_definiton(MESH_TYPE_VTC);
	vb = malloc(def->stride * vertices_count);
	display_gl_generate_box(&min, &max, 8.0f, 0xffffffff, vb, def);
	display->sky_box = display_gl_mesh_vtc_create(vb, vertices_count, NULL, 0);
	free(vb);
}

void display_gl_sky_update(struct vm_s* vm, struct display_gl_s* display) {

}

void display_gl_sky_render(struct vm_s* vm, struct display_gl_s* display) {
	v4_t								color_diffuse;
	v4_t								color_ambient;
	mat4_t							local;
	mat4_t							projection;

	quat_t							quat;


	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	display->sky_box_angle += (float) display->frame_delta / 70.f;

	quat_from_euler(&quat, display->sky_box_angle, display->sky_box_angle, 0.0f);
	quat_to_mat4(&quat, &local);

	mat4_ident(&projection);
	mat4_perspective(&projection, 70.0f, display->frame_buffer_ratio,-100.0f, 0.0f);
	// mat4_ortho(&projection, -0.5f, 0.5f, 0.5f, -0.5f, -10.0f, 0.0f);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, display->sky_box_texture);
	glDisable(GL_BLEND);
	glCullFace(GL_FRONT);

	display_gl_mesh_render_start(display->mesh_renderer, MESH_TYPE_VTC);
	display_gl_mesh_set_ambient(display->mesh_renderer, &color_ambient);
	display_gl_mesh_set_diffuse(display->mesh_renderer, &color_diffuse);
	display_gl_mesh_set_local(display->mesh_renderer, &local);
	display_gl_mesh_set_projection(display->mesh_renderer, &projection);
	display_gl_mesh_render(display->sky_box);
}

void display_gl_sky_destroy(struct display_gl_s* display) {
	display_gl_mesh_destroy(display->sky_box);
	glDeleteTextures(1, &display->sky_box_texture);
}
