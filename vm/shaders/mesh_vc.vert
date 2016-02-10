#version 140

uniform mat4	uni_ProjectionMatrix;
uniform mat4	uni_LocalMatrix;
uniform vec4	uni_Diffuse;

in	vec3		in_Position;
in	vec4		in_Color;

out vec4		vertexShader_Color;

void main(void)
{
  vertexShader_Color = clamp(in_Color * uni_Diffuse, vec4(0.0), vec4(1.0));
	gl_Position = uni_ProjectionMatrix * uni_LocalMatrix * vec4(in_Position, 1.0);
}
