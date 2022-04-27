#include "camera.h"
#include "controls.h"
#include "pgr.h"
#include "data.h"
#include "object.h"
#include "scene.h"

extern struct GameState gameState;

Camera::Camera(){
	freeCamera = false;

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	position = glm::vec3(0.5f, 0.5f, 0.5f);
	front = glm::vec3(1.0f, 0.0f, 1.0f);

	yaw = 135.0f;
	pitch = -20.0f;

	update();
}

void Camera::setView1() {
	freeCamera = false;

	worldUp = glm::vec3(0.0f, 0.0f, -1.0f);

	position = glm::vec3(1.0f, 1.0f, 1.0f);
	front = glm::vec3(1.0f, 0.0f, 1.0f);

	yaw = 270.0f;
	pitch = 0.0f;

	update();
}

void Camera::setView2() {
	freeCamera = false;

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(1.0f, 0.0f, 1.0f);

	yaw = 135.0f;
	pitch = -20.0f;

	update();
}

void Camera::update() {
	glm::vec3 newDirection;
	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pithc));
	direction = glm::normalize(newDirection);

	right = glm::normalize(glm::cross(direction, worldUp));		// -x
	up = glm::normalize(glm::cross(right, direction));			// +y?
}

glm::mat4 Camera::viewMatrix() {
	return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::projectionMatrix() {
	// Angle, aspect, near, far
	return glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 25.0f);
}

void Camera::checkBounds() {
	if (position.x > 10.0f)
		position.x = 10.0f;
	if (position.y > 5.0f)
		position.y = 5.0f;
	if (position.z > 10.0f)
		position.z = 10.0f;
	if (position.x < -10.0f)
		position.x = -10.0f;
	if (position.y < -5.0f)
		position.y = -5.0f;
	if (position.z < -10.0f)
		position.z = -10.0f;
}

void Camera::changePosition() {
	if (!freeCamera)
		return;

	float speed = CAMERA_SPEED; /* gameState.elapsedTime;*/
	if (!objectCollision(glm::vec3(0.736f, 0.0f, -0.85f), 0.2f) && !objectCollision(glm::vec3(0.0f, 0.0f, 0.0f), 0.2f) 
		&& !objectCollision(glm::vec3(0.325f, 0.0f, 0.8f), 0.2f) && !objectCollision(glm::vec3(-0.4f, 0.0f, -0.15f), 0.2f)
		&& !objectCollision(glm::vec3(0.47f, 0.15f, 1.3f), 0.2f)){
		if (gameState.keyMap[KEY_UP_ARROW])
			position += direction * speed;
		if (gameState.keyMap[KEY_DOWN_ARROW])
			position -= direction * speed;
		if (gameState.keyMap[KEY_LEFT_ARROW])
			position -= right * speed;
		if (gameState.keyMap[KEY_RIGHT_ARROW])
			position += right * speed;
	}
	checkBounds();
}

void Camera::changeAngles(float x, float y) {
	x *= CAMERA_SENSITIVITY;
	y *= CAMERA_SENSITIVITY;

	yaw += x;
	pitch -= y;

	update();
}


bool Camera::objectCollision(const glm::vec3& position1, float r) {
	glm::vec3 tmp = position - position1;

	float distance = length(tmp);
	float radius = PLAYER_RADIUS + r;

	if (distance < radius)
		return true;
	return false;
}
