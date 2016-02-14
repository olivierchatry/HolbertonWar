#version 140

uniform mat4	uni_ProjectionMatrix;
uniform mat4	uni_LocalMatrix;
uniform vec4	uni_Diffuse;

in	vec3		in_Position;
in  vec2    in_UV;
in	vec4		in_Color;

out vec2    vertexShader_UV;
out vec4		vertexShader_Color;

void main(void)
{
  vertexShader_UV = in_UV;
  vertexShader_Color = clamp(in_Color * uni_Diffuse, vec4(0.0), vec4(1.0));
	gl_Position = uni_ProjectionMatrix * uni_LocalMatrix * vec4(in_Position, 1.0);
}
