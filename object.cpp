#include <iostream>
#include "object.h"
#include "render.h"
#include "controls.h"

extern struct RegularShader		regularShader;
extern struct SkyboxShader		skyboxShader;
extern struct GameState			gameState;

// delete object geometry
void Object::cleanupGeometry() {
	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

	if (geometry->texture != 0)
		glDeleteTextures(1, &(geometry->texture));
}

bool Skybox::initGeometry()
{
	geometry = new Geometry;

	// 2D coordinates of 2 triangles covering the whole screen (NDC), draw using triangle strip
	static const float screenCoords[] = {
	  -1.0f, -1.0f,
	   1.0f, -1.0f,
	  -1.0f,  1.0f,
	   1.0f,  1.0f
	};

	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	// Far plane rendering buffer
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	glUseProgram(skyboxShader.program);

	glEnableVertexAttribArray(skyboxShader.screenCoordLocation);
	glVertexAttribPointer(skyboxShader.screenCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();

	geometry->numTriangles = 2;

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &(geometry->texture));
	glBindTexture(GL_TEXTURE_CUBE_MAP, geometry->texture);

	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = "data/skybox/" + std::to_string(i) + ".png";
		pgr::loadTexImage2D(texName, targets[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();

	return false;
}

void Skybox::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
	glUseProgram(skyboxShader.program);

	glm::mat4 matrix = projectionMatrix * viewMatrix;
	glm::mat4 viewRotation = viewMatrix;

	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

	CHECK_GL_ERROR();
	glUniformMatrix4fv(skyboxShader.inversePVmatrixLocation, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxShader.skyboxSamplerLocation, 0);

	CHECK_GL_ERROR();
	// draw "skybox" rendering 2 triangles covering the far plane
	glBindVertexArray(geometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, geometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, geometry->numTriangles + 2);
	CHECK_GL_ERROR();
	glBindVertexArray(0);
	glUseProgram(0);
}

bool StaticObject::initGeometry() {
	// Assimp importer
	Assimp::Importer importer;

	//importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Loading assets from file
	const aiScene* scn = importer.ReadFile(filePath.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// Abort if the loader fails
	if (scn == NULL) {
		std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// Multiple meshes in one file are not supported
	if (scn->mNumMeshes != 1) {
		std::cerr << "Cannot process files with more than one mesh." << std::endl;
		return false;
	}

	const aiMesh* mesh = scn->mMeshes[0];

	geometry = new Geometry;

	// VBO
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// Vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// Normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// Textures are 2D
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// Storing all faces (triangles) in an array
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	glGenBuffers(1, &(geometry->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// Saving material information
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name);

	// Diffuse
	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.5f);
	geometry->diffuse = glm::vec3(color.r, color.g, color.b);

	// Ambient
	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.5f);
	geometry->ambient = glm::vec3(color.r, color.g, color.b);

	// Specular
	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.5f);
	geometry->specular = glm::vec3(color.r, color.g, color.b);

	ai_real glossiness, strength;
	unsigned int max;

	// Glossiness
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &glossiness, &max)) != AI_SUCCESS)
		glossiness = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	geometry->glossiness = glossiness * strength;

	geometry->texture = 0;

	// Loading texture
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString path;

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = filePath.find_last_of("/\\");
		if (found != std::string::npos) {
			textureName.insert(0, filePath.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		geometry->texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	glEnableVertexAttribArray(regularShader.posLocation);
	glVertexAttribPointer(regularShader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(regularShader.normalLocation);
	glVertexAttribPointer(regularShader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

	glEnableVertexAttribArray(regularShader.texCoordLocation);
	glVertexAttribPointer(regularShader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));

	CHECK_GL_ERROR();

	glBindVertexArray(0);

	geometry->numTriangles = mesh->mNumFaces;

	return true;
}

bool StaticObject::initHardcodedGeometry()
{
	geometry = new Geometry;
	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redpepperVertices), redpepperVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(regularShader.posLocation);
	glVertexAttribPointer(regularShader.posLocation, 3, GL_FLOAT, GL_FALSE, redpepperNAttribsPerVertex * sizeof(float), 0);

	glEnableVertexAttribArray(regularShader.normalLocation);
	glVertexAttribPointer(regularShader.normalLocation, 3, GL_FLOAT, GL_FALSE, redpepperNAttribsPerVertex * sizeof(float), (void*)(3 * sizeof(float)));

	glGenBuffers(1, &(geometry->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(redpepperTriangles), redpepperTriangles, GL_STATIC_DRAW);

	glBindVertexArray(0);

	geometry->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	geometry->diffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	geometry->specular = glm::vec3(0.7f, 0.6253f, 0.6f);
	geometry->glossiness = 0.25f;
	geometry->texture = 0;

	glBindVertexArray(0);

	geometry->numTriangles = redpepperNTriangles;

	return true;
}

void StaticObject::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glStencilFunc(GL_ALWAYS, id, -1);

	glUseProgram(regularShader.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
	modelMatrix = glm::mat4(1.0f) * modelMatrix;
	modelMatrix = glm::rotate(modelMatrix, direction, glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(size * 1.5));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		geometry->ambient,
		geometry->diffuse,
		geometry->specular,
		geometry->glossiness,
		geometry->texture,
		multi,
		transparent
	);

	glBindVertexArray(geometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, geometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}