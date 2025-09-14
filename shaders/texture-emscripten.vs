#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out highp vec2 TexCoord;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;
uniform highp float texture_scale;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x * texture_scale, aTexCoord.y * texture_scale);
}