#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 Fragcolor;

uniform Material material;
// uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D outTexture;

void main() {
	//float ambientLight = 0.5;
	//vec3 ambient = lightColor * ambientLight;
	vec3 ambient = lightColor * material.ambient;
	
	vec3 normalVector = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = lightColor * (diff * material.diffuse);

	//int shininess = 128;
	//float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);

	//vec3 result = (ambient + diffuse + specular) * objectColor;
	vec3 result = (ambient + diffuse + specular);
	
	Fragcolor = vec4(result, 1.0);
	Fragcolor = texture(outTexture, TexCoord) * Fragcolor;
}