#version 140

uniform mat4	uni_ProjectionMatrix;
uniform vec2	uni_Coord[4];
uniform vec4	uni_Color;

in	vec4		in_Position;
in  float		in_Value;

out vec2		vertexShader_UV;
out vec4		vertexShader_Color;

void main(void)
{
	vec4 position = vec4(in_Position.xyz, 1.0);
	vertexShader_UV = uni_Coord[int(in_Position.w)];
	vertexShader_UV.x = vertexShader_UV.x + in_Value / 256.0;
	float alpha = ((in_Value / 255.0) * 0.5) + 0.5;
	vertexShader_Color = vec4(uni_Color.xyz * alpha, 1);
	gl_Position = uni_ProjectionMatrix * position;
}
