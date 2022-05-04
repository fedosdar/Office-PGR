#include <iostream>
#include "render.h"
#include "object.h"
#include "controls.h"

struct SkyboxShader			skyboxShader;
struct RegularShader		regularShader;
extern struct GameState		gameState;

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix){
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(regularShader.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));

	glUniformMatrix4fv(regularShader.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(regularShader.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	const glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

	glUniformMatrix4fv(regularShader.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float glossiness, GLuint texture, bool multi, bool transparent){
	// Material information
	glUniform3fv(regularShader.diffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(regularShader.ambientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(regularShader.specularLocation, 1, glm::value_ptr(specular));
	glUniform1f(regularShader.glossinessLocation, glossiness);

	if (texture) {
		// There is a texture which needs to be set up
		glUniform1i(regularShader.texSamplerLocation, 0);
		glUniform1i(regularShader.textureLocation, 1);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texture);


		// Multi texture (multiple textures in one image)
		if (multi)
			glUniform1i(regularShader.multiLocation, 1);
		else
			glUniform1i(regularShader.multiLocation, 0);
	}
	else {
		// There is no texture
		glUniform1i(regularShader.textureLocation, 0);
	}

	// Transparent object
	if (transparent)
		glUniform1i(regularShader.transparentLocation, 1);
	else
		glUniform1i(regularShader.transparentLocation, 0);

	// Daytime toggle
	if (gameState.day)
		glUniform1i(regularShader.dayLocation, 1);
	else
		glUniform1i(regularShader.dayLocation, 0);

	// Flashlight toggle
	if (gameState.reflector)
		glUniform1i(regularShader.reflectorLocation, 1);
	else
		glUniform1i(regularShader.reflectorLocation, 0);

	// Monitor light toggle
	if (gameState.pointLight)
		glUniform1i(regularShader.pointLightLocation, 1);
	else
		glUniform1i(regularShader.pointLightLocation, 0);
}

void initSkyboxShader() {
	std::vector<GLuint> shaderList;

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skyboxShader.frag"));
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skyboxShader.vert"));

	// create the program with two shaders
	skyboxShader.program = pgr::createProgram(shaderList);

	// uniforms locations
	skyboxShader.screenCoordLocation = glGetAttribLocation(skyboxShader.program, "screenCoord");
	skyboxShader.inversePVmatrixLocation = glGetUniformLocation(skyboxShader.program, "inversePVmatrix");
	skyboxShader.skyboxSamplerLocation = glGetUniformLocation(skyboxShader.program, "skyboxSampler");
}

void initRegularShader() {
	std::vector<GLuint> shaderList;

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "regularShader.frag"));
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "regularShader.vert"));

	// create the program with two shaders
	regularShader.program = pgr::createProgram(shaderList);

	// attribute locations
	regularShader.posLocation = glGetAttribLocation(regularShader.program, "position");
	regularShader.normalLocation = glGetAttribLocation(regularShader.program, "normal");
	regularShader.texCoordLocation = glGetAttribLocation(regularShader.program, "texCoord");

	// general uniform locations
	regularShader.PVMmatrixLocation = glGetUniformLocation(regularShader.program, "PVMmatrix");
	regularShader.VmatrixLocation = glGetUniformLocation(regularShader.program, "Vmatrix");
	regularShader.MmatrixLocation = glGetUniformLocation(regularShader.program, "Mmatrix");
	regularShader.normalMatrixLocation = glGetUniformLocation(regularShader.program, "normalMatrix");
	regularShader.timeLocation = glGetUniformLocation(regularShader.program, "time");

	// material information locations
	regularShader.diffuseLocation = glGetUniformLocation(regularShader.program, "material.diffuse");
	regularShader.ambientLocation = glGetUniformLocation(regularShader.program, "material.ambient");
	regularShader.specularLocation = glGetUniformLocation(regularShader.program, "material.specular");
	regularShader.glossinessLocation = glGetUniformLocation(regularShader.program, "material.glossiness");

	// Texture information locations
	regularShader.texSamplerLocation = glGetUniformLocation(regularShader.program, "texSampler");
	regularShader.textureLocation = glGetUniformLocation(regularShader.program, "material.texture");
	regularShader.multiLocation = glGetUniformLocation(regularShader.program, "multi");

	// Special information locations
	regularShader.dayLocation = glGetUniformLocation(regularShader.program, "day");
	regularShader.reflectorLocation = glGetUniformLocation(regularShader.program, "reflector");
	regularShader.transparentLocation = glGetUniformLocation(regularShader.program, "transparent");
	regularShader.pointLightLocation = glGetUniformLocation(regularShader.program, "pointLight");
}	

void initShaderPrograms(){
	initRegularShader();
	initSkyboxShader();
}

void cleanupShaderPrograms(){
	pgr::deleteProgramAndShaders(skyboxShader.program);
	pgr::deleteProgramAndShaders(regularShader.program);
}