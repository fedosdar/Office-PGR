#pragma once

#include "pgr.h"
#include "object.h"

class Camera {
	
public:
	bool freeCamera;

	glm::vec3 position;			// Camera position
	glm::vec3 direction;		
	glm::vec3 front;				
	glm::vec3 right;			
	glm::vec3 worldUp;			// World up-vector

	float yaw;					// Horizontal angle
	float pitch;				// Vertical angle

	// vec3 direction
	// direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	// direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// direction.y = sin(glm::radians(pithc));

	Camera();

	bool objectCollision(const glm::vec3& cameraPos, const glm::vec3& objectPos, float r);

	void setView1();
	void setView2();

	void update();

	glm::mat4 viewMatrix();
	glm::mat4 projectionMatrix();

	void checkBounds();
	void changePosition();
	void changeAngles(float x, float y);

};