#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;

uniform sampler2D texture1;
uniform vec4 color;

void main()
{
	vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
	FragColor = color * texColor;
}