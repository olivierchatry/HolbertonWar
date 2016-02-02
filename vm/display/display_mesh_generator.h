#ifndef DISPLAY_MESH_GENERATOR_H
#define DISPLAY_MESH_GENERATOR_H

#include	"../../common/holberton-core.h"

struct v3_s;
typedef struct v3_s v3_t;

void display_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count);
void display_generate_sphere(int subDiv, v3_t* center, float sphere_radius, uint8* vb, mesh_definition_t* def, uint16* ib, uint16 start);


void display_generate_rect_count(int32 subDiv, int32* vertex_count, int32* index_count);
void display_generate_rect(int subDiv, v3_t* min, v3_t* max, float sphere_radius, uint8* vb, mesh_definition_t* def, uint16* ib, uint16 start);

#endif
