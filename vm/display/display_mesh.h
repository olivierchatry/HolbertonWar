#ifndef DISPLAY_MESH_H
#define DISPLAY_MESH_H

#include	"../../common/holberton-core.h"

#define MESH_TYPE_VN	0
#define MESH_TYPE_V		1
#define MESH_TYPE_VC	2
#define MESH_TYPE_VTC	3
#define MESH_TYPE_COUNT 4

struct s_mesh_definition
{
	int32 vertex_offset;
	int32 normal_offset;
	int32 uv_offset;
	int32 color_offset;
	int32 stride;
};

struct s_display_mesh_renderer;
struct s_mesh;

typedef struct s_mesh t_mesh;
typedef struct s_display_mesh_renderer t_display_mesh_renderer;
typedef struct s_mesh_definition t_mesh_definition;

t_mesh_definition*				display_mesh_get_definiton(int type);
t_display_mesh_renderer*	display_mesh_renderer_initialize();
void											display_mesh_renderer_destroy(t_display_mesh_renderer* renderer);
void											display_mesh_render_start(t_display_mesh_renderer* renderer, int type);
void											display_mesh_render(t_mesh* mesh);
t_mesh*										display_mesh_vn_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
t_mesh*										display_mesh_v_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
t_mesh*										display_mesh_vc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
t_mesh*										display_mesh_vtc_create(void* vertices, int32 vertex_count, uint16* indices, int32 index_count);
void											display_mesh_set_light_direction(t_display_mesh_renderer* renderer, v3_t* direction);
void											display_mesh_set_ambient(t_display_mesh_renderer* renderer, v4_t* color);
void											display_mesh_set_diffuse(t_display_mesh_renderer* renderer, v4_t* color);
void											display_mesh_set_projection(t_display_mesh_renderer* renderer, mat4_t* projection);
void											display_mesh_set_local(t_display_mesh_renderer* renderer, mat4_t* local);
void											display_mesh_set_normal(t_display_mesh_renderer* renderer, mat4_t* normal);
void											display_mesh_destroy(t_mesh* mesh);
int32											display_mesh_get_vb(t_mesh* mesh);
void											display_mesh_render_count(t_mesh* mesh, int32 count);

#endif
