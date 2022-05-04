#include "camera.h"
#include "controls.h"
#include "pgr.h"
#include "data.h"
#include "object.h"

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

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	position = glm::vec3(0.0f, 3.0f, 0.0f);
	front = glm::vec3(0.0f, -1.0f, 0.0f);

	yaw = 0.0f;
	pitch = -90.0f;

	update();
}

void Camera::setView2() {
	freeCamera = false;

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	position = glm::vec3(-0.28f, 1.1f, -0.12f);
	direction = glm::vec3(2.0f, -0.2f, 1.0f);

	yaw = 60.0f;
	pitch = -36.0f;

	update();
}

void Camera::update() {
	glm::vec3 newDirection;
	newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	newDirection.y = sin(glm::radians(pitch));
	direction = glm::normalize(newDirection);

	right = glm::normalize(glm::cross(direction, worldUp));		// -x
	front = glm::normalize(glm::cross(right, direction));		// +y?
}

glm::mat4 Camera::viewMatrix() {
	return glm::lookAt(position, position + direction, front);
}

glm::mat4 Camera::projectionMatrix() {
	// Angle, aspect, near, far
	return glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 25.0f);
}

void Camera::checkBounds() {
	if (position.x > 1.35f)
		position.x = 1.35f;
	if (position.y > 2.85f)
		position.y = 2.85f;
	if (position.z > 1.35f)
		position.z = 1.35f;
	if (position.x < -1.35f)
		position.x = -1.35f;
	if (position.y < 0.15f)
		position.y = 0.15f;
	if (position.z < -1.35f)
		position.z = -1.35f;
}

void Camera::changePosition() {
	if (!freeCamera)
		return;

	float speed = CAMERA_SPEED; /* gameState.elapsedTime;*/
	glm::vec3 newPosition;	

	if (gameState.keyMap[KEY_UP_ARROW])
		newPosition = position + direction * speed;
	if (gameState.keyMap[KEY_DOWN_ARROW])
		newPosition = position - direction * speed;
	if (gameState.keyMap[KEY_LEFT_ARROW])
		newPosition = position - right * speed;
	if (gameState.keyMap[KEY_RIGHT_ARROW])
		newPosition = position + right * speed;

	if (!objectCollision(newPosition, glm::vec3(0.736f, 0.0f, -0.85f), 0.4f) && !objectCollision(newPosition, glm::vec3(0.325f, 0.0f, 0.8f), 0.4f)				// armchair & chair
		&& !objectCollision(newPosition, glm::vec3(-0.4f, 0.0f, -0.15f), 0.7f) && !objectCollision(newPosition, glm::vec3(0.6f, 0.39f, 1.34f), 0.0001f)		// lama & monitor
		&& !objectCollision(newPosition, glm::vec3(-0.5f, 2.0f, 0.0f), 0.0001f) && !objectCollision(newPosition, glm::vec3(0.2f, 0.41f, 1.32f), 0.0001f)		// paperplane & printer
		&& !objectCollision(newPosition, glm::vec3(0.47f, 0.15f, 1.3f), 0.0001f) && !objectCollision(newPosition, glm::vec3(0.75f, 0.365f, 1.24f), 0.0001f))	// table & tissuebox
		position = newPosition;
	
	
	checkBounds();
}

void Camera::changeAngles(float x, float y) {
	x *= CAMERA_SENSITIVITY;
	y *= CAMERA_SENSITIVITY;

	yaw += x;
	pitch -= y;

	update();
}


bool Camera::objectCollision(const glm::vec3& cameraPos, const glm::vec3& objectPos, float r) {
	
	float distance = length(cameraPos - objectPos);
	//float radius = PLAYER_RADIUS + r;

	if (distance < r)
		return true;
	return false;
}
