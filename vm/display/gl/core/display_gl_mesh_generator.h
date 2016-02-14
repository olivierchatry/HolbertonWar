#ifndef DISPLAY_MESH_GENERATOR_H
#define DISPLAY_MESH_GENERATOR_H

#include	"../../common/holberton-core.h"
#include "display_gl_mesh.h"
#include "display_gl_math.h"


void display_gl_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count);
void display_gl_generate_sphere(int subDiv, v3_t* center, float sphere_radius, int8* vb, mesh_definition_t* def, uint16* ib, uint16 start);

void		display_gl_generate_line_count(int32* vertex_count);
int8*	display_gl_generate_line(v3_t* min, v3_t* max, float size_start, float size_end, uint32 color, int8* vb, mesh_definition_t* def);

void	display_gl_generate_rect_count(int32* vertex_count);
int8* display_gl_generate_rect(v3_t* min, v3_t* max, uint32 color, int8* vb, mesh_definition_t* def);

#endif
