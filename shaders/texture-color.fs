#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec3 fragPos;

uniform sampler2D texture1;
uniform vec4 color;
uniform vec3 playerPos;

uniform float lightRadius = 6.0;
uniform float lightDistanceDetail = 2.0;
uniform float lightRingsCount = 2.0;

void main()
{
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

    float distance = length(playerPos - fragPos) / lightRadius;
    int idist = int(distance * lightDistanceDetail);
    
    vec3 norm = normalize(Normals);
    vec3 lightDir = normalize(playerPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    int idiff = int(diff * lightRingsCount);
    vec3 diffuse = vec3(float(idiff) / lightRingsCount, float(idiff) / lightRingsCount, float(idiff) / lightRingsCount) * lightColor.xyz;
    distance = float(idist) / lightDistanceDetail;
    lightColor = vec4(diffuse * (1.0 - distance), 1.0);

	vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
	FragColor = color * texColor * lightColor;
}