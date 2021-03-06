#include <GL/glew.h>
#include <stdlib.h>

#include "display_gl_math.h"
#include "display_gl_mesh.h"
#include "display_gl_helpers.h"


static int8* display_gl_generate(int8* vb, mesh_definition_t* def, const float* vs, const float* uvs, const int32* indices, int32 indices_count, uint32 color) {
	int32 i;
	int32 stride = uvs ? 3 : 5;
	for (i = 0; i < indices_count; ++i) {
		v3_t* v = (v3_t*)(vb + def->vertex_offset);
		const float* vsp = vs + indices[i] * stride;
		v3_set(v, *vsp, *(vsp + 1), *(vsp + 2));

		if (def->uv_offset != -1) {
			v2_t* uv = (v2_t*)(vb + def->uv_offset);
			if (uvs) {
				v2_set(uv, *(uvs + 0), *(uvs + 1));
				uvs += 2;
			} else {
				v2_set(uv, *(vsp + 3), *(vsp + 4));
			}
		}

		if (def->color_offset != -1) {
			uint32* c = (uint32*)(vb + def->color_offset);
			*c = color;
		}
		vb += def->stride;
	}
	return vb;
}

void display_gl_generate_rect_count(int32* vertex_count)
{
	*vertex_count = 6;
}

int8* display_gl_generate_rect(v3_t* min, v3_t* max, uint32 color, int8* vb, mesh_definition_t* def)
{
	const float  vs[] = {
		min->x, min->y, min->z, 0.0f, 0.0f,
		max->x, min->y, min->z, 1.0f, 0.0f,
		min->x, max->y, min->z, 0.0f, 1.0f,
		max->x, max->y, min->z, 1.0f, 1.0f
	};

	const int32 indices[] = {
		0, 1, 3, 0, 3, 2
	};

	return display_gl_generate(vb, def, vs, NULL, indices, 6, color);
}

void display_gl_generate_line_count(int32* vertex_count)
{
	*vertex_count = 6;
}


int8* display_gl_generate_line(v3_t* min, v3_t* max, float size_start, float size_end, uint32 color, int8* vb, mesh_definition_t* def)
{
	v3_t direction;
	v3_t normal, normal_end;

	v3_set(&normal, 0, 0, 1);
	v3_sub(max, min, &direction);
	v3_norm(&direction, &direction);
	v3_cross(&direction, &normal, &normal);
	v3_norm(&normal, &normal);

	size_start *= 0.5f;
	size_end *= 0.5f;

	v3_set(&normal_end, normal.x, normal.y, normal.z);
	normal.x *= size_start;
	normal.y *= size_start;
	normal.z *= size_start;

	normal_end.x *= size_end;
	normal_end.y *= size_end;
	normal_end.z *= size_end;

	const float  vs[] = {
		min->x - normal.x, min->y - normal.y, min->z, 0.0f, 0.0f,
		min->x + normal.x, min->y + normal.y, min->z, 1.0f, 0.0f,
		max->x - normal_end.x, max->y - normal_end.y, max->z, 0.0f, 1.0f,
		max->x + normal_end.x, max->y + normal_end.y, max->z, 1.0f, 1.0f
	};

	const int32 indices[] = {
		0, 1, 3, 0, 3, 2
	};

	return display_gl_generate(vb, def, vs, NULL, indices, 6, color);
}

void display_gl_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count) {
	*vertex_count = (subDiv + 1) * subDiv;
	*index_count = subDiv * 6 * subDiv;
}

void v3_polar(float phi, float theta, float radius, v3_t* out) {
	out->x = radius * cosf(theta) * sinf(phi);
	out->y = radius * sinf(theta) * sinf(phi);
	out->z = radius * cosf(phi);
}

void display_gl_generate_sphere(int subDiv, v3_t* center, float sphere_radius, int8* vb, mesh_definition_t* def, uint16* ib, uint16 start)
{
	int32 i = subDiv + 1;
	float delta = DISPLAY_M_PI / (subDiv);

	float phi= 0.0f;
	int32 count = 0;
	int32 subDivJ = subDiv;

	while (i --)
	{
		float  theta = 0.0f;
		int32 j = subDivJ;
		while (j --)
		{
			v3_t*	v = (v3_t*)(vb + def->vertex_offset);
			v3_t*	n = (v3_t*)(vb + def->normal_offset);

			v3_polar(phi, theta * 2.0f, sphere_radius, v);
			v3_norm(v, n);
			v3_add(v, center, v);

			vb += def->stride;
			theta += delta;
			count++;
		}
		phi += delta;
	}

	uint16* tmp = ib;
	for (i = 0; i < subDiv; ++i)
	{
		int32 j;
		for (j = 0; j < subDivJ; ++j)
		{
			int32 i0 = start + (i + 0) * subDivJ + j;
			int32 i1 = start + (i + 1) * subDivJ + j;
			int32 i2 = start + (i + 1) * subDivJ + ((j + 1) % subDivJ);
			int32 i3 = start + (i + 0) * subDivJ + ((j + 1) % subDivJ);

			*tmp++ = i0, *tmp++ = i1, *tmp++ = i2;
			*tmp++ = i0, *tmp++ = i2, *tmp++ = i3;
		}
	}
}

void display_gl_generate_arrow_count(int32* vertex_count)
{
	*vertex_count = 6;
}


int8* display_gl_generate_arrow(v3_t* min, v3_t* max, float size, uint32 color, int8* vb, mesh_definition_t* def) {
	v3_t direction;
	v3_t normal;
	v3_t middle;
	float middle_pos = 0.9f;

	v3_set(&normal, 0, 0, 1);
	v3_sub(max, min, &direction);
	v3_set(&middle, direction.x * middle_pos, direction.y * middle_pos, direction.z * middle_pos);
	v3_add(&middle, min, &middle);

	v3_norm(&direction, &direction);
	v3_cross(&direction, &normal, &normal);
	v3_norm(&normal, &normal);

	size *= 0.5f;

	normal.x *= size;
	normal.y *= size;
	normal.z *= size;


	const float  vs[] = {
		min->x - normal.x, min->y - normal.y, min->z, 0.0f, 0.0f,
		middle.x, middle.y, middle.z, 0.0, middle_pos,
		min->x + normal.x, min->y + normal.y, min->z, 1.0f, 0.0f,
		middle.x - normal.x, middle.y - normal.y, middle.z, 0.0f, middle_pos,
		max->x, max->y, max->z, 0.0f, 1.0f,
		middle.x + normal.x, middle.y + normal.y, middle.z, 1.0f, middle_pos
	};
	const int32 indices[] = {
		0,1,2,3,4,5
	};

	return display_gl_generate(vb, def, vs, NULL, indices, 6, color);
}


void display_gl_generate_box_count(int32* vertex_count) {
	*vertex_count = 6 * 6;
}


int8* display_gl_generate_box(v3_t* min, v3_t* max, float uv_repeat, uint32 color, int8* vb, mesh_definition_t* def) {
	const int32 indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		3, 2, 6,
		6, 7, 3,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// left
		4, 0, 3,
		3, 7, 4,
		// right
		1, 5, 6,
		6, 2, 1,
	};
	const float uvs[] = {
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
		0.0f, 0.0f, uv_repeat, 0.0f, uv_repeat, uv_repeat, uv_repeat, uv_repeat, 0.0f, uv_repeat, 0.0f, 0.0f,
	};

	const float vs[] = {
		min->x, min->y, max->z, 
		max->x, min->y, max->z, 
		max->x, max->y, max->z, 
		min->x, max->y, max->z, 
		min->x, min->y, min->z, 
		max->x, min->y, min->z, 
		max->x, max->y, min->z, 
		min->x, max->y, min->z,
	};

	return display_gl_generate(vb, def, vs, uvs, indices, 36, color);

}