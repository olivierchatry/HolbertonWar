#include <GL/glew.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <stb_easy_font.h>

#include "display_gl_def.h"
#include "display_gl_math.h"
#include "display_gl_text.h"
#include "display_gl_mesh.h"

#define MAX_TEXT		100
#define MAX_TEXT_LEN	256

typedef struct s_text
{
	v2_t	position;
	int32	rgba;
	char*	text;

} t_text;

typedef struct display_gl_text_s
{
	t_text						texts[MAX_TEXT];
	int32						text_count;

	display_gl_mesh_renderer_t*	text_renderer;
	mesh_t*						text_mesh;
	uint8*						text_mesh_vb;
	uint32						text_mesh_vb_size;
} display_gl_text_t;


display_gl_text_t* display_gl_text_intialize()
{
	display_gl_text_t* text = malloc(sizeof(display_gl_text_t));
	uint16*			ib;
	uint16*			temp;
	int32			size;
	int32			i, start;

	text->text_count = 0;
	text->text_renderer = display_gl_mesh_renderer_initialize();

	size = MAX_TEXT * MAX_TEXT_LEN * 6;
	temp = ib = malloc(size * sizeof(uint16));
	start = 0;
	for (i = 0; i < MAX_TEXT * MAX_TEXT_LEN; i += 6)
	{
		*temp++ = start + 0;
		*temp++ = start + 1;
		*temp++ = start + 3;
		*temp++ = start + 1;
		*temp++ = start + 3;
		*temp++ = start + 2;
		start += 4;
	}


	mesh_definition_t* def = display_gl_mesh_get_definiton(MESH_TYPE_VC);
	text->text_mesh_vb_size = MAX_TEXT * MAX_TEXT_LEN * 4 * def->stride;
	text->text_mesh = display_gl_mesh_vc_create(NULL, text->text_mesh_vb_size, ib, size);
	free(ib);

	text->text_mesh_vb = malloc(text->text_mesh_vb_size);
	return text;
}

void			display_gl_text_clear(display_gl_text_t* texts)
{
	int32 i;
	for (i = 0; i < texts->text_count; ++i)
	{
		free(texts->texts[i % MAX_TEXT].text);
	}

	texts->text_count = 0;
}

void			display_gl_text_destroy(display_gl_text_t* texts)
{
	int32 i;
	int32 count = MAX_TEXT;

	if (count > texts->text_count)
		count = texts->text_count;

	for (i = 0; i < count; ++i)
		free(texts->texts[i].text);

	display_gl_mesh_destroy(texts->text_mesh);
	display_gl_mesh_renderer_destroy(texts->text_renderer);

	free(texts->text_mesh_vb);
	free(texts);


}


float	display_gl_text_add_va(display_gl_text_t* texts, float x, float y, int32 rgba, char* format, va_list args) {
	char*	buffer = malloc(MAX_TEXT_LEN);
	vsnprintf(buffer, MAX_TEXT_LEN, format, args);
	texts->texts[texts->text_count % MAX_TEXT].text = buffer;
	texts->texts[texts->text_count % MAX_TEXT].rgba = rgba;
	texts->texts[texts->text_count % MAX_TEXT].position.x = x;
	texts->texts[texts->text_count % MAX_TEXT].position.y = y;
	texts->text_count++;
	return stb_easy_font_height(buffer) + y + 1;
}

float	display_gl_text_add(display_gl_text_t* texts, float x, float y, int32 rgba, char* format, ...)
{
	va_list args;
	float ret;

	va_start(args, format);
	ret = display_gl_text_add_va(texts, x, y, rgba, format, args);
	va_end(args);
	return ret;
}


void			display_gl_text_render(display_gl_text_t* texts, mat4_t* projection_view)
{
	int32 i;
	int32 vb_index = 0;
	v4_t	color_diffuse;
	v4_t	color_ambient;
	mat4_t	local;
	v4_set(&color_diffuse, 1.f, 1.f, 1.0f, 1.0f);
	v4_set(&color_ambient, 0.0f, 0.0f, 0.0f, 0.0f);
	mat4_ident(&local);

	for (i = 0; i < texts->text_count; ++i)
	{
		t_text* text = &(texts->texts[i]);

		vb_index += stb_easy_font_print(text->position.x,
			text->position.y,
			text->text,
			(uint8*)&text->rgba,
			texts->text_mesh_vb + vb_index,
			texts->text_mesh_vb_size) * 64;
	}
	glBindBuffer(GL_ARRAY_BUFFER, display_gl_mesh_get_vb(texts->text_mesh));
	glBufferSubData(GL_ARRAY_BUFFER, 0, vb_index, texts->text_mesh_vb);

	display_gl_mesh_render_start(texts->text_renderer, MESH_TYPE_VC);
	display_gl_mesh_set_ambient(texts->text_renderer, &color_ambient);
	display_gl_mesh_set_diffuse(texts->text_renderer, &color_diffuse);
	display_gl_mesh_set_projection(texts->text_renderer, projection_view);
	display_gl_mesh_set_local(texts->text_renderer, &local);
	display_gl_mesh_render_count(texts->text_mesh, (vb_index / 64) * 6);
}
