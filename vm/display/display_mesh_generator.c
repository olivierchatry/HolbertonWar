#include <GL/glew.h>
#include <stdlib.h>

#include "display_math.h"
#include "display_mesh.h"
#include "display_gl.h"

void display_generate_rect_count(int32 subDiv, int32* vertex_count, int32* index_count)
{
	*vertex_count = 4;
	*index_count = 6;
}

void display_generate_rect(int subDiv, v3_t* min, v3_t* max, float sphere_radius, uint8* vb, mesh_definition_t* def, uint16* ib, uint16 start)
{
	float  vs[] = {
		min->x, min->y, min->z,
		max->x, min->y, min->z,
		min->x, max->y, min->z,
		max->x, max->y, min->z
	};
	float* vsp = vs;
	int32 i;

	for (i = 0; i < 4; ++i)
	{
		v3_t* v = (v3_t*)(vb + def->vertex_offset);
		v3_t* n = (v3_t*)(vb + def->normal_offset);
		v3_set(v, *vsp, *(vsp + 1), *(vsp + 2));
		vsp += 3;
		if (def->normal_offset != -1)
			v3_set(n, 0.0f, 0.0f, 1.0f);
		vb += def->stride;
	}

	*ib++ = start + 0;
	*ib++ = start + 1;
	*ib++ = start + 3;

	*ib++ = start + 0;
	*ib++ = start + 3;
	*ib++ = start + 2;
}


void display_generate_line(int subDiv, v3_t* min, v3_t* max, float sphere_radius, uint8* vb, mesh_definition_t* def, uint16* ib, uint16 start)
{
	v3_t direction;
	v3_t normal;

	v3_set(&normal, 0, 0, 1);
	v3_sub(max, min, &direction);
	v3_norm(&direction, &direction);
	v3_cross(&direction, &normal, &normal);

	normal.x *= sphere_radius;
	normal.y *= sphere_radius;
	normal.z *= sphere_radius;

	float  vs[] = {
		min->x - normal.x, min->y - normal.y, min->z,
		min->x + normal.x, min->y + normal.y, min->z,
		max->x - normal.x, max->y - normal.y, max->z,
		max->x + normal.x, max->y + normal.y, max->z,
	};

	float* vsp = vs;
	int32 i;

	for (i = 0; i < 4; ++i)
	{
		v3_t* v = (v3_t*)(vb + def->vertex_offset);
		v3_t* n = (v3_t*)(vb + def->normal_offset);
		v3_set(v, *vsp, *(vsp + 1), *(vsp + 2));
		vsp += 3;
		if (def->normal_offset != -1)
			v3_set(n, 0.0f, 0.0f, 1.0f);
		vb += def->stride;
	}

	*ib++ = start + 0;
	*ib++ = start + 1;
	*ib++ = start + 3;

	*ib++ = start + 0;
	*ib++ = start + 3;
	*ib++ = start + 2;
}


void display_generate_sphere_count(int32 subDiv, int32* vertex_count, int32* index_count)
{
	*vertex_count = (subDiv + 1) * subDiv;
	*index_count = subDiv * 6 * subDiv;
}

void v3_polar(float phi, float theta, float radius, v3_t* out)
{
	out->x = radius * cosf(theta) * sinf(phi);
	out->y = radius * sinf(theta) * sinf(phi);
	out->z = radius * cosf(phi);
}

void display_generate_sphere(int subDiv, v3_t* center, float sphere_radius, uint8* vb, mesh_definition_t* def, uint16* ib, uint16 start)
{
	int32 i = subDiv + 1;
	float delta = DISPLAY_M_PI / (subDiv);

	float phi= 0.0f;
	int32 count = 0;
	v3_t* vbtmp = (v3_t*)vb;
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
	i = 1;
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
