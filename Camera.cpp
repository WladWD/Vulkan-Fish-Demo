#include "Camera.h"


Engine::Camera::Camera(float fov, float aspectRatio, float near, float far):
	fov(glm::pi<float>() / 2.0f), aspectRatio(1.0f), near(near), far(far),
	proj(glm::mat4(1.0f)), view(glm::mat4(1.0f)), projView(glm::mat4(1.0f)),
	cameraPosition(glm::vec3(0.0f)), cameraForward(glm::vec3(0.0f, 0.0f, 1.0f)),
	cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)), cameraRight(glm::vec3(1.0f, 0.0f, 0.0f)) {
	updateMatrix();
}

void Engine::Camera::updateMatrix(void) {
	//TODO add function
}

void Engine::Camera::resize(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	proj = glm::perspectiveFov(fov, aspectRatio, 1.0f, near, far);
	projView = proj * view;
}

float Engine::Camera::getFov(void) const {
	return fov;
}

float Engine::Camera::getAspectRatio(void) const {
	return aspectRatio;
}

float Engine::Camera::getFarPlane(void) const {
	return far;
}

float Engine::Camera::getNearPlane(void) const {
	return near;
}

glm::vec3 Engine::Camera::getCameraWorldPosition(void) const {
	return cameraPosition;
}

glm::vec3 Engine::Camera::getCameraForward(void) const {
	return cameraForward;
}

glm::mat4 Engine::Camera::getViewMatrix(void) const {
	return view;
}

glm::mat4 Engine::Camera::getProjMatrix(void) const {
	return proj;
}

glm::mat4 Engine::Camera::getProjViewMatrix(void) const {
	return projView;
}

void Engine::Camera::setCameraPosition(const glm::vec3 & position) {
	cameraPosition = position;
	updateMatrix();
}

void Engine::Camera::setCameraDirection(const glm::vec3 & forward, const glm::vec3 &up) {
	cameraForward = forward;
	cameraUp = up;
	cameraRight = glm::cross(cameraUp, cameraForward);
	updateMatrix();
}
