#ifndef DISPLAY_GL_H
#define DISPLAY_GL_H

#include	"../../common/holberton-core.h"

typedef struct shader_s
{
	int id;
	int vertex_id;
	int fragment_id;
} shader_t;

#define DISPLAY_GL_ERROR_SHADER_VERT_FILE -1
#define DISPLAY_GL_ERROR_SHADER_FRAG_FILE -2
#define DISPLAY_GL_OK					   1

void	display_gl_log(int id, const char* desc, int is_shader);
void	display_gl_destroy_shader(shader_t* shader);
int32	display_gl_compile_shader(char* name, char* src, int32 type);
int32	display_gl_load_shader(shader_t* shader, char* vert_file, char* frag_file);
int32	display_gl_create_buffer(int32 type, int32 size, int32 flags, void* data);
int32	display_gl_load_texture(char* file_name);
int32	display_gl_create_vao();

#endif
