#version 300 es
out highp vec4 FragColor;

in highp vec2 TexCoords;
in highp vec3 Normals;

uniform sampler2D texture1;
uniform highp vec4 color;

void main()
{
	highp vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
	FragColor = color * texColor;
}