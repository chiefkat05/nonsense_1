#version 300 es
out highp vec4 FragColor;

in highp vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	vec4 highp texColor = texture(texture1, TexCoord);
    if(texColor.a < 0.1)
        discard;
	FragColor = texColor;
}