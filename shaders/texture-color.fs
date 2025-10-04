#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec3 fragPos;

uniform sampler2D texture1;
uniform vec4 color;
uniform vec3 playerPos;

void main()
{
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    float distance = length(playerPos - fragPos) + 2.0;
    int idist = int(distance) * 5;
    
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(playerPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    int idiff = int(diff * 200.0);
    vec3 diffuse = vec3(float(idiff) * 0.02, float(idiff) * 0.02, float(idiff) * 0.02) * vec3(lightColor);
    lightColor = vec4(diffuse / (float(idist) * 0.2), 1.0);

	vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
	FragColor = color * texColor * lightColor;
}