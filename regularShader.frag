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

	// Spot light
    vec3  direction;
	float cosHalfAngle;
	float exponent;

	// Point light
	vec3  attenuation;
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



vec4 directionalLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

	vec3 ret = vec3(0.0f);

	vec3 L = normalize(light.position);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));

	ret += material.ambient * light.ambient;
	ret += material.diffuse * light.diffuse * NdotL;
	ret += material.specular * light.specular * pow(RdotV, material.glossiness);

	return vec4(ret, 1.0);
}

vec4 spotLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

	vec3 ret = vec3(0.0f);

	vec3 L = normalize(light.position - vertexPosition);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));
	float coef = max(0.0, dot(-L, light.direction));

	ret += material.ambient * light.ambient;
	ret += material.diffuse * light.diffuse * NdotL;
	ret += material.specular * light.specular * pow(RdotV, material.glossiness);

	if(coef < light.cosHalfAngle)
		ret *= 0.0;
	else
		ret *= pow(coef, light.exponent);

	ret *= vec3(1.0f, 1.0f, 0.6f);

	return vec4(ret,  1.0);
}

vec4 pointLight(Light light, Material material, vec3 vertexPosition, vec3 vertexNormal) {

	vec3 ret = vec3(0.0f);

	vec3 L = normalize(light.position - vertexPosition);
	vec3 R = reflect(-L, vertexNormal);
	vec3 V = normalize(-vertexPosition);

	float NdotL = max(0.0, dot(vertexNormal, L));
	float RdotV = max(0.0, dot(R, V));

	vec3 ambi = material.ambient * light.ambient;
	vec3 diff = material.diffuse * light.diffuse * NdotL;
	vec3 spec = material.specular * light.specular * pow(RdotV, material.glossiness);

	vec3 pos = (Vmatrix * vec4(light.position, 1.0f)).xyz;

	float dis = distance(vertexPosition, pos);

	float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * dis + light.attenuation.z * pow(dis, 2));
	ret += (ambi + diff + spec) * attenuation * vec3(1.0f, 0.7f, 0.0f);

	return vec4(ret,  1.0);
}

// Hardcoded lights
Light sun;
Light spotlight;

void initLights() {

	// Sun parameters (directional light)
	if(day) {
		sun.ambient  = vec3(0.3f);
		sun.diffuse  = vec3(1.0f);
		sun.specular = vec3(1.0f);

		sun.position = (Vmatrix * vec4(cos(time * 0.2f), 0.0f, sin(time * 0.2f), 0.0f)).xyz;
	}

	if(reflector) {
		// Lighthouse spotlight parameters (spot light)
		spotlight.ambient = vec3(0.1f);
		spotlight.diffuse = vec3(1.0f);
		spotlight.specular = vec3(1.0f);
		spotlight.cosHalfAngle = 0.995f;
		spotlight.exponent  = 0.0f;
	
		spotlight.position = (Vmatrix * vec4(11.5606f, 3.88163f, -0.352651f, 1.0f)).xyz;
		spotlight.direction = (Vmatrix * vec4(cos(time), 0.0f, sin(time), 0.0f)).xyz;
	}
}

vec4 fog(vec4 colour_f) {
	float cameraDistance = length(Vmatrix * Mmatrix * vec4(position_v, 1.0f));
	float intensity = 0.03f;
	float blend = clamp(exp(-pow(cameraDistance * intensity, 2)), 0.0f, 1.0f);
	
	return colour_f * blend + (1 - blend) * vec4(1.0f, 0.4f, 0.8f, 1.0f);
}

void main() {

    initLights();

	// Eye coordinates position and vertex normal
    vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position_v, 1.0)).xyz;
    vec3 vertexNormal = normalize((Vmatrix * normalMatrix * vec4(normal_v, 0.0)).xyz);

	// Output colour with global ambient initialization
    vec3 globalAmbientLight = vec3(0.3f);
    vec4 outColour = vec4(material.ambient * globalAmbientLight, 1.0);

    // Accumulating contribution from all lights
	if(day)
		outColour += directionalLight(sun, material, vertexPosition, vertexNormal);
	if(reflector)
		outColour += spotLight(spotlight, material, vertexPosition, vertexNormal);

    // Applying texture
	if(material.texture) {
		// Applying texture
		if(multi) {
			int frame = int(10 * time) % 8; // 32 frames total - 8 per row, 4 per column
			vec2 offset = vec2((1.0f / 4) * (frame % 4), (1.0f / 2) * int(frame / 2));
			colour_f = outColour * texture(texSampler, texCoord_v + offset);
		}
		else
			colour_f = outColour * texture(texSampler, texCoord_v);
	}
	else
		colour_f = outColour;

	colour_f = fog(colour_f);
}
