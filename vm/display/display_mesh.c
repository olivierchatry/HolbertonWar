#include <GL/glew.h>
#include <stdlib.h>
#include "display_math.h"
#include "display_mesh.h"
#include "display_gl.h"

typedef struct s_mesh_vn
{
	v3_t	v;
	v3_t	n;
}	mesh_vn_t;


typedef struct s_mesh_v
{
	v3_t	v;
}	t_mesh_v;

typedef struct s_mesh_vc
{
	v3_t	v;
	uint32	argb;
}	t_mesh_vc;

typedef struct s_mesh_vtc
{
	v3_t		v;
	v2_t		uv;
	uint32	argb;
}	t_mesh_vtc;


typedef struct s_mesh
{
	int32	vao;
	int32	vb;
	int32	ib;

	int32	count;
	int32	type;
} t_mesh;

typedef struct s_display_mesh_shader
{
	shader_t	shader;
	int32		uniform_projection_matrix;
	int32		uniform_diffuse;
	int32		uniform_light_direction;
	int32		uniform_local_matrix;
	int32	  uniform_normal_matrix;
	int32		uniform_ambient;

} t_display_mesh_shader;


typedef struct s_display_mesh_renderer
{
	t_display_mesh_shader	type[MESH_TYPE_COUNT];
	t_display_mesh_shader*	current;
} t_display_mesh_renderer;

static t_mesh_definition	s_definition_table[] =
{
	{ 0,  3 * sizeof(float), -1,                     -1,                6 * sizeof(float)} ,
	{ 0, -1,                 -1,                     -1,                3 * sizeof(float) },
	{ 0, -1,                 -1,                     3 * sizeof(float), 3 * sizeof(float) + sizeof(uint32)},
	{ 0, -1,                  3 * sizeof(float),     5 * sizeof(float), 5 * sizeof(float) + sizeof(uint32) },

};


t_mesh_definition* display_mesh_get_definiton(int type)
{
	return &(s_definition_table[type]);
}

t_display_mesh_renderer* display_mesh_renderer_initialize()
{

	location_t			location_vn[] = { { "in_Position", 0 }, { "in_Normal", 1 }, { NULL, 0 } };
	location_t			location_v[] = { { "in_Position", 0 },{ NULL, 0 } };
	location_t			location_vc[] = { { "in_Position", 0 },{ "in_Color", 1 },{ NULL, 0 } };
	location_t			location_vtc[] = { { "in_Position", 0 },{ "in_UV", 1 }, { "in_Color", 2 },{ NULL, 0 } };

	t_display_mesh_renderer* renderer = (t_display_mesh_renderer*)malloc(sizeof(t_display_mesh_renderer));
	display_gl_load_shader(&renderer->type[MESH_TYPE_VN].shader, "shaders/mesh_vn.vert", "shaders/mesh_vn.frag", location_vn);
	int32 id = renderer->type[MESH_TYPE_VN].shader.id;
	renderer->type[MESH_TYPE_VN].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_VN].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_VN].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_VN].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_VN].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");
	renderer->type[MESH_TYPE_VN].uniform_normal_matrix = glGetUniformLocation(id, "uni_NormalMatrix");

	display_gl_load_shader(&renderer->type[MESH_TYPE_V].shader, "shaders/mesh_v.vert", "shaders/mesh_v.frag", location_v);
	id = renderer->type[MESH_TYPE_V].shader.id;
	renderer->type[MESH_TYPE_V].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_V].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_V].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_V].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_V].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");

	display_gl_load_shader(&renderer->type[MESH_TYPE_VC].shader, "shaders/mesh_vc.vert", "shaders/mesh_vc.frag", location_vc);
	id = renderer->type[MESH_TYPE_VC].shader.id;
	renderer->type[MESH_TYPE_VC].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_VC].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_VC].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_VC].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_VC].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");

	display_gl_load_shader(&renderer->type[MESH_TYPE_VTC].shader, "shaders/mesh_vtc.vert", "shaders/mesh_vtc.frag", location_vtc);
	id = renderer->type[MESH_TYPE_VTC].shader.id;
	renderer->type[MESH_TYPE_VTC].uniform_projection_matrix = glGetUniformLocation(id, "uni_ProjectionMatrix");
	renderer->type[MESH_TYPE_VTC].uniform_light_direction = glGetUniformLocation(id, "uni_LightDirection");
	renderer->type[MESH_TYPE_VTC].uniform_diffuse = glGetUniformLocation(id, "uni_Diffuse");
	renderer->type[MESH_TYPE_VTC].uniform_ambient = glGetUniformLocation(id, "uni_Ambient");
	renderer->type[MESH_TYPE_VTC].uniform_local_matrix = glGetUniformLocation(id, "uni_LocalMatrix");
	
	GLuint sampler = glGetUniformLocation(id, "uni_Sampler");
	GLint		samplerId = 1;
	glUniform1iv(sampler, 1, &samplerId);

	return renderer;
}

void	display_mesh_renderer_destroy(t_display_mesh_renderer* renderer)
{
	int i;
	for (i = 0; i < MESH_TYPE_COUNT; ++i)
	{
		display_gl_destroy_shader(&renderer->type[i].shader);
	}

	free(renderer);
}

t_mesh* display_mesh_vn_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_VN;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(mesh_vn_t), GL_STATIC_DRAW, vertices);
	if (index_count > 0) {
		mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	}
	else {
		mesh->ib = 0;
	}
	mesh->vao = display_gl_create_vao();
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VN);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->normal_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return mesh;
}

t_mesh* display_mesh_vtc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_VTC;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_vtc), GL_STATIC_DRAW, vertices);
	if (index_count > 0) {
		mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	}
	else {
		mesh->ib = 0;
	}
	mesh->vao = display_gl_create_vao();
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VTC);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->uv_offset);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, def->stride, (const void*)def->color_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	return mesh;
}

t_mesh* display_mesh_vc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_VC;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_vc), GL_STATIC_DRAW, vertices);
	if (index_count > 0) {
		mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	}
	else {
		mesh->ib = 0;
	}
	mesh->vao = display_gl_create_vao();
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_VC);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, def->stride, (const void*)def->color_offset);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return mesh;
}

t_mesh* display_mesh_v_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count)
{
	t_mesh* mesh = malloc(sizeof(t_mesh));
	mesh->count = index_count;
	mesh->type = MESH_TYPE_V;
	mesh->vb = display_gl_create_buffer(GL_ARRAY_BUFFER, vertex_count * sizeof(t_mesh_v), GL_STATIC_DRAW, vertices);
	if (index_count > 0) {
		mesh->ib = display_gl_create_buffer(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16), GL_STATIC_DRAW, indices);
	}
	else {
		mesh->ib = 0;
	}
	mesh->vao = display_gl_create_vao();
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
	t_mesh_definition* def = display_mesh_get_definiton(MESH_TYPE_V);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, def->stride, (const void*)def->vertex_offset);
	glEnableVertexAttribArray(0);
	return mesh;
}


void	display_mesh_destroy(t_mesh* mesh)
{
	glDeleteBuffers(1, &mesh->ib);
	glDeleteBuffers(1, &mesh->vb);
	glDeleteVertexArrays(1, &mesh->vao);
	free(mesh);
}


void display_mesh_render_start(t_display_mesh_renderer* renderer, int type)
{
	renderer->current = &(renderer->type[type]);
	glUseProgram(renderer->current->shader.id);
}

void display_mesh_render(t_mesh* mesh)
{
	glBindVertexArray(mesh->vao);

	if (mesh->ib) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
		glDrawElements(GL_TRIANGLES, mesh->count, GL_UNSIGNED_SHORT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, mesh->count);
		glBindVertexArray(0);
	}

}

void display_mesh_render_count(t_mesh* mesh, int32 count) {
	glBindVertexArray(mesh->vao);
	if (mesh->ib) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ib);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, count);
		glBindVertexArray(0);
	}
}

void display_mesh_set_ambient(t_display_mesh_renderer* renderer, v4_t* color)
{
	glUniform4fv(renderer->current->uniform_ambient, 1, (float*) color);
}

void display_mesh_set_diffuse(t_display_mesh_renderer* renderer, v4_t* color)
{
	glUniform4fv(renderer->current->uniform_diffuse, 1, (float*)color);
}

void display_mesh_set_projection(t_display_mesh_renderer* renderer, mat4_t* projection)
{
	glUniformMatrix4fv(renderer->current->uniform_projection_matrix, 1, GL_FALSE, projection->mat.v);
}

int32 display_mesh_get_ib(t_mesh* mesh)
{
	return mesh->ib;
}

int32 display_mesh_get_vb(t_mesh* mesh)
{
	return mesh->vb;
}

void display_mesh_set_normal(t_display_mesh_renderer* renderer, mat4_t* normal)
{
	glUniformMatrix4fv(renderer->current->uniform_normal_matrix, 1, GL_FALSE, normal->mat.v);
}

void display_mesh_set_local(t_display_mesh_renderer* renderer, mat4_t* local)
{
	glUniformMatrix4fv(renderer->current->uniform_local_matrix, 1, GL_FALSE, local->mat.v);
}

void display_mesh_set_light_direction(t_display_mesh_renderer* renderer, v3_t* direction)
{
	glUniform3fv(renderer->current->uniform_light_direction, 1, (float*) direction);
}
