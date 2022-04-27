#pragma once

#include "pgr.h"
#include "object.h"

struct RegularShader {
	// Shader program ID
	GLuint program;

	// Attribute locations
	GLint posLocation;
	GLint colorLocation;
	GLint normalLocation;
	GLint texCoordLocation;

	// General uniform locations
	GLint PVMmatrixLocation;
	GLint VmatrixLocation;
	GLint MmatrixLocation;
	GLint normalMatrixLocation;
	GLint timeLocation;

	// Material information locations 
	GLint diffuseLocation;
	GLint ambientLocation;
	GLint specularLocation;
	GLint glossinessLocation;

	// Texture information locations
	GLint texSamplerLocation;
	GLint textureLocation;
	GLint multiLocation;

	// Special information locations
	GLint dayLocation;
	GLint reflectorLocation;
};

struct SkyboxShader {
	// Shader program ID
	GLuint program;

	// Vertex attribute locations
	GLint screenCoordLocation;

	// Uniform locations
	GLint inversePVmatrixLocation;
	GLint skyboxSamplerLocation;
};

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float glossiness, GLuint texture, bool multi);

void initSkyboxShader();
void initRegularShader();

void initShaderPrograms();
void cleanupShaderPrograms();