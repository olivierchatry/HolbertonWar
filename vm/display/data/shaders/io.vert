#version 140

uniform mat4	uni_ProjectionMatrix;
uniform vec4	uni_Color;
const vec2 const_Coord[4]=vec2[4](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 1.0)
);

in	vec4		in_Position;
in  float		in_Value;

out vec4		vertexShader_Color;
out vec2    vertexShader_UV;

void main(void)
{
  vec4 position = vec4(in_Position.xyz, 1.0);
  vertexShader_UV = const_Coord[int(in_Position.w)];

	float v = ceil(in_Value / 255.0);
  vertexShader_Color = vec4(uni_Color.xyz,  clamp(v, 0.3, 0.8) * ceil(v));
	gl_Position = uni_ProjectionMatrix * position;
}
