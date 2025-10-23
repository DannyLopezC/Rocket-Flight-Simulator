#include "Camera.h"


Camera::Camera()
{
	position = glm::vec3(0,0,0);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = 0;
	pitch = 0;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	movementSpeed = 0;
	turnSpeed = 0;
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch,
	GLfloat startMovementSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	movementSpeed = startMovementSpeed;
	turnSpeed = startTurnSpeed;
	
	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = movementSpeed * deltaTime;

	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	// Ignore unrealistic spikes
	if (fabs(xChange) > 100.0f || fabs(yChange) > 100.0f)
		return;

	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)  pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	// keep yaw bounded
	if (yaw > 360.0f) yaw -= 360.0f;
	else if (yaw < 0.0f) yaw += 360.0f;

	update();
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::update()
{
	//printf("front x = %f, front y = %f, front z = %f, yaw = %f \n", front.x, front.y, front.z, yaw);
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera()
{

}
