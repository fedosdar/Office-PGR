#version 140

uniform samplerCube skyboxSampler;

in vec3 texCoord;
out vec4 color;

void main() {
	color = texture(skyboxSampler, texCoord);
}