#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormals;

out vec2 TexCoords;
out vec3 Normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 texture_scale;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	Normals = vec3(aNormals.x, aNormals.y, aNormals.z);
	if (Normals.x != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.y, aTexCoords.y * texture_scale.z);
	}
	if (Normals.y != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.x, aTexCoords.y * texture_scale.z);
	}
	if (Normals.z != 0.0)
	{
		TexCoords = vec2(aTexCoords.x * texture_scale.x, aTexCoords.y * texture_scale.y);
	}
}