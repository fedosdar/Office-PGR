#version 140

// Material information
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float glossiness;
	bool texture;					
};

in vec3 position;					
in vec3 normal;						
in vec2 texCoord;					

out vec3 position_v;				
out vec3 normal_v;					
out vec2 texCoord_v;				

uniform Material material;

uniform mat4 Vmatrix;				
uniform mat4 Mmatrix;				
uniform mat4 PVMmatrix;				

void main() {

    gl_Position = PVMmatrix * vec4(position, 1.0f);

    position_v = position;
    normal_v = normal;
	texCoord_v = texCoord;
}
