#version 140

uniform vec4		uni_Ambient;

in vec4 vertexShader_Color;
out vec4 out_FragColor;


void main(void)
{
    out_FragColor = clamp(vertexShader_Color + uni_Ambient, vec4(0.0), vec4(1.0));
}
