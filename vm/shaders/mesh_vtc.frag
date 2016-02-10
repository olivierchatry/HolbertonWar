#version 140

uniform sampler2D uni_Sampler;
uniform vec4		  uni_Ambient;

in vec2   vertexShader_UV;
in vec4   vertexShader_Color;

out vec4  out_FragColor;


void main(void)
{
  vec4 texColour = texture(uni_Sampler, vertexShader_UV);
  out_FragColor = texColour * clamp(vertexShader_Color + uni_Ambient, vec4(0.0), vec4(1.0));
}
