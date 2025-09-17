#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormals;

out highp vec2 TexCoords;
out highp vec3 Normals;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;
uniform highp vec3 texture_scale;
uniform highp vec2 texture_pixel_scale;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
	Normals = vec3(aNormals.x, aNormals.y, aNormals.z);
	if (Normals.x != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.y * texture_pixel_scale.x, aTexCoords.y * texture_scale.z * texture_pixel_scale.y);
	}
	if (Normals.y != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.x * texture_pixel_scale.x, aTexCoords.y * texture_scale.z * texture_pixel_scale.y);
	}
	if (Normals.z != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.x * texture_pixel_scale.x, aTexCoords.y * texture_scale.y * texture_pixel_scale.y);
	}
}