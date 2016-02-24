#ifndef DISPLAY_MATH_H
#define DISPLAY_MATH_H

#include <math.h>

#define LERP(s, e, a) ( s * a ) + ( e * (1.0f - a) )
struct v4_s
{
	float x;
	float y;
	float z;
	float w;
};


struct v3_s
{
	float x;
	float y;
	float z;
};

struct v2_s
{
	float x;
	float y;
};

struct mat4_s
{
	union
	{
		float v[16];
		float m[4][4];
	} mat;
};


typedef struct v4_s v4_t;
typedef struct v4_s quat_t;

typedef struct v3_s v3_t;
typedef struct v2_s v2_t;
typedef struct mat4_s mat4_t;

void	quat_from_euler(quat_t* out, float yaw, float pitch, float rool);
void	quat_normalize(quat_t* out);
void	quat_to_mat4(quat_t* in, mat4_t* out);

float		mat4_det(mat4_t* in);
int			mat4_invert(mat4_t* in, mat4_t* out);
void		mat4_ident(mat4_t* mat);
void		mat4_ortho(mat4_t* mat, float l, float r, float b, float t, float n, float f);
void		mat4_perspective(mat4_t* mat, float fovy, float aspect, float near, float far);
int			mat4_transpose(mat4_t* in, mat4_t* out);
void		mat4_x_v3(mat4_t* mat, v3_t* src, v3_t* dst);
void		mat4_translate(mat4_t* mat, float x, float y, float z);
mat4_t*	mat4_mul(mat4_t* mat, mat4_t* with, mat4_t* out);

v3_t*	v3_set(v3_t* v, float x, float y, float z);
v3_t* v3_sub(v3_t* a, v3_t* b, v3_t* out);
float	v3_norm(v3_t* vin, v3_t* vout);
v3_t*	v3_add(v3_t* vin, v3_t* add, v3_t* vout);
v3_t* v3_cross(v3_t* a, v3_t* b, v3_t* out);
float v3_len(v3_t* v);

v2_t*	v2_set(v2_t* v, float x, float y);
v2_t*	v2_add(v2_t* vin, v2_t* add, v2_t* vout);
v2_t*	v2_sub(v2_t* vin, v2_t* sub, v2_t* vout);
v2_t*	v2_mul(v2_t* vin, float sx, float sy, v2_t* vout);


v4_t*	v4_set(v4_t* v, float x, float y, float z, float w);
#define DISPLAY_M_PI       3.14159265358979f

#endif
