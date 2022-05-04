#version 140

// Material information
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float glossiness;
	bool texture;					
};

struct Light {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;

    vec3  position;

    vec3  direction;		// Reflector & directional
	
	float cosHalfAngle;		// Reflector
	
	float exponent;			

	vec3  attenuation;		// Point(const, lin, quad) & Reflector
};

in vec3 position_v;					// Position (in)
in vec3 normal_v;					// Normal (in)
in vec2 texCoord_v;					// Texture coordinates (in)

out vec4 colour_f;					// Fragment colour (out)

uniform Material material;
uniform sampler2D texSampler;

uniform mat4 Vmatrix;
uniform mat4 Mmatrix;
uniform mat4 normalMatrix;

uniform float time;					// Used for texture movement
uniform bool multi;					// Multi texture

uniform bool day;
uniform bool reflector;
uniform bool transparent;
uniform bool pointLight;

// Directional light
vec4 directionalLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {
	vec3 result = vec3(0.0f);

	vec3 L = normalize(light.position);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));

	result += material.ambient * light.ambient;
	result += material.diffuse * light.diffuse * NdotL;
	result += material.specular * light.specular * pow(RdotV, material.glossiness);

	return vec4(result, 1.0);
}

// Point light
vec4 pointlight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {
	vec3 result = vec3(0.0f);

	vec3 L = normalize(light.position - vertexPosition);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));

	vec3 amb = material.ambient * light.ambient;
	vec3 diff = material.diffuse * light.diffuse * NdotL;
	vec3 spec = material.specular * light.specular * pow(RdotV, material.glossiness);

	float distance = length(light.position - vertexPosition);

	float attenuation = 1.0f / ( light.attenuation.x 
								+ light.attenuation.y * distance
								+ light.attenuation.z * distance * distance);

	result += attenuation * (amb + diff + spec);

	return vec4(result, 1.0f);
}

// Spot light
vec4 spotLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {
	vec3 result = vec3(0.0f);

	vec3 L = normalize(light.position - vertexPosition);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));
	float coef = max(0.0, dot(-L, light.direction));

	vec3 amb = material.ambient * light.ambient;
	vec3 diff = material.diffuse * light.diffuse * NdotL;
	vec3 spec = material.specular * light.specular * pow(RdotV, material.glossiness);

	if (coef > light.cosHalfAngle) {
		float distance = length(light.position - vertexPosition);

		float attenuation = 1.0f / ( light.attenuation.x 
									+ light.attenuation.y * distance
									+ light.attenuation.z * distance * distance);
		
		result += attenuation * (amb + diff + spec);
	}
	else {
		result *= 0.0f;
	}
	//result *= vec3(1.0f, 1.0f, 0.6f);
	return vec4(result, 1.0);
}

Light sun;
Light flashlight;
Light monitorLight;

// Init all lights
void initLights() {
	if(day) {
		sun.ambient = vec3(0.3f);
		sun.diffuse = vec3(1.0f);
		sun.specular = vec3(1.0f);

		sun.position = (Vmatrix * vec4(0.0f, 3.0f, 0.0f, 1.0f)).xyz;
		sun.direction = (Vmatrix * vec4(-0.2f, -1.0f, -0.3f, 0.0f)).xyz;
	}

	if(reflector) {
		flashlight.ambient = vec3(0.1f);
		flashlight.diffuse = vec3(0.8f);
		flashlight.specular = vec3(1.0f);

		flashlight.attenuation = vec3(1.0f, 0.09f, 0.032f);
		flashlight.cosHalfAngle = 0.9f;

		flashlight.position = (Vmatrix * vec4(0.0f, 2.5f, 0.0f, 1.0f)).xyz;
		flashlight.direction = (Vmatrix * vec4(0.0f, -1.0f, 0.0f, 0.0f)).xyz;
	}

	if(pointLight) {
		monitorLight.ambient = vec3(0.2f);
		monitorLight.diffuse = vec3(0.5f);
		monitorLight.specular = vec3(1.0f);

		monitorLight.attenuation = vec3(1.0f, 0.35f, 0.44f);
		
		monitorLight.position = (Vmatrix * vec4(0.6f, 0.39f, 1.34f, 1.0f)).xyz;
	}
}

vec4 fog(vec4 colour_f) {
	float cameraDistance = length(Vmatrix * Mmatrix * vec4(position_v, 1.0f));
	float intensity = 0.1f;
	float blend = clamp(exp(-pow(cameraDistance * intensity, 2)), 0.0f, 1.0f);

	return colour_f * blend + (1 - blend) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void main() {

    initLights();

	// Eye coordinates position and vertex normal
    vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position_v, 1.0f)).xyz;
    vec3 vertexNormal = normalize((Vmatrix * normalMatrix * vec4(normal_v, 0.0f)).xyz);

	// Output colour with global ambient initialization
    vec3 globalAmbientLight = vec3(0.3f);
    vec4 outColour = vec4(material.ambient * globalAmbientLight, 1.0f);

    // Accumulating contribution from all lights
	if(day)
		outColour += directionalLight(sun, material, vertexPosition, vertexNormal);
	if(reflector)
		outColour += spotLight(flashlight, material, vertexPosition, vertexNormal);
	if(pointLight)
		outColour += pointlight(monitorLight, material, vertexPosition, vertexNormal);

    // Applying texture
	if(material.texture) {
		// Applying multi texture
		if(multi) {
			int frame = int(10 * time) % 8; // 8 frames total - 4 per row, 2 per column
			vec2 offset = vec2((1.0f / 4) * (frame % 4), (1.0f / 2) * int(frame / 2));
			colour_f = outColour * texture(texSampler, texCoord_v + offset);
		}
		else
			colour_f = outColour * texture(texSampler, texCoord_v);
	}
	else
		colour_f = outColour;

	colour_f = fog(colour_f);

	if (transparent)
		colour_f.a = 0.7f;
}
