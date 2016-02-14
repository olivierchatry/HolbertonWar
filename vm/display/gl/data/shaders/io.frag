#version 140

uniform sampler2D uni_Texture;


in vec4 vertexShader_Color;
in vec2 vertexShader_UV;
out vec4 out_FragColor;


void main(void)
{
    out_FragColor = texture(uni_Texture, vertexShader_UV) * vertexShader_Color;
}
