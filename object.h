#pragma once

#include <string>
#include <vector>
#include "pgr.h"

struct Geometry {

	unsigned int numTriangles;

	GLuint vertexBufferObject;
	GLuint vertexArrayObject;
	GLuint elementBufferObject;

	// Material
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float glossiness;
	GLuint texture;

};

class Object {

public:
	Geometry* geometry = NULL;

	Object() {};
	virtual bool	initGeometry() = 0;
	void			cleanupGeometry();

};

class Skybox : public Object {

public:
	bool initGeometry();
	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

};

class StaticObject : public Object {

public:
	unsigned char id;

	std::string filePath;
	glm::vec3 position;
	float size;
	float direction;		// In radians

	bool multi = false;

	StaticObject(unsigned char id, std::string filePath, glm::vec3 position, float size, float direction) :
		id(id),
		filePath(filePath),
		position(position),
		size(size),
		direction(direction) {};

	StaticObject(unsigned char id, std::string filePath, glm::vec3 position, float size, float direction, bool multi) :
		id(id),
		filePath(filePath),
		position(position),
		size(size),
		direction(direction),
		multi(multi) {};


	bool initGeometry();

	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

};