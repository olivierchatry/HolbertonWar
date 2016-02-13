#ifndef DISPLAY_MESH_H
#define DISPLAY_MESH_H

#include	"../../common/holberton-core.h"
#include "display_math.h"

#define MESH_TYPE_VN	0
#define MESH_TYPE_V		1
#define MESH_TYPE_VC	2
#define MESH_TYPE_VTC	3
#define MESH_TYPE_COUNT 4

struct mesh_definition_s
{
	int32 vertex_offset;
	int32 normal_offset;
	int32 uv_offset;
	int32 color_offset;
	int32 stride;
};

struct display_mesh_renderer_s;
struct mesh_s;
struct v3_s;
struct v4_s;
struct mat4_s;

typedef struct v3_s v3_t;
typedef struct v4_s v4_t;
typedef struct mat4_s mat4_t;

typedef struct mesh_s mesh_t;
typedef struct display_mesh_renderer_s display_mesh_renderer_t;
typedef struct mesh_definition_s mesh_definition_t;

mesh_definition_t*				display_mesh_get_definiton(int type);
display_mesh_renderer_t*	display_mesh_renderer_initialize();
void											display_mesh_renderer_destroy(display_mesh_renderer_t* renderer);
void											display_mesh_render_start(display_mesh_renderer_t* renderer, int type);
void											display_mesh_render(mesh_t* mesh);
mesh_t*										display_mesh_vn_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
mesh_t*										display_mesh_v_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
mesh_t*										display_mesh_vc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
mesh_t*										display_mesh_vtc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
void											display_mesh_set_light_direction(display_mesh_renderer_t* renderer, v3_t* direction);
void											display_mesh_set_ambient(display_mesh_renderer_t* renderer, v4_t* color);
void											display_mesh_set_diffuse(display_mesh_renderer_t* renderer, v4_t* color);
void											display_mesh_set_projection(display_mesh_renderer_t* renderer, mat4_t* projection);
void											display_mesh_set_local(display_mesh_renderer_t* renderer, mat4_t* local);
void											display_mesh_set_normal(display_mesh_renderer_t* renderer, mat4_t* normal);
void											display_mesh_destroy(mesh_t* mesh);
int32											display_mesh_get_vb(mesh_t* mesh);
void											display_mesh_render_count(mesh_t* mesh, int32 count);

#endif
