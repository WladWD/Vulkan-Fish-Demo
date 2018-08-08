#include "Camera.h"


Engine::Camera::Camera(float fov, float aspectRatio, float near, float far):
	fov(glm::pi<float>() / 2.0f), aspectRatio(1.0f), near(near), far(far),
	proj(glm::mat4(1.0f)), view(glm::mat4(1.0f)), projView(glm::mat4(1.0f)),
	cameraPosition(glm::vec3(0.0f)), cameraForward(glm::vec3(0.0f, 0.0f, 1.0f)),
	cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)), cameraRight(glm::vec3(1.0f, 0.0f, 0.0f)) {
	updateMatrix();
}

void Engine::Camera::updateMatrix(void) {
	view[0] = glm::vec4(cameraRight, 0.0f);
	view[1] = glm::vec4(cameraUp, 0.0f);
	view[2] = glm::vec4(cameraForward, 0.0f);
	view[3] = glm::vec4(-cameraPosition, 0.0f);

	projView = proj * view;
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
	cameraForward = glm::normalize(forward);
	cameraUp = cameraUp - cameraForward * glm::dot(cameraForward, cameraUp);
	cameraUp = glm::normalize(up);
	cameraRight = glm::cross(cameraUp, cameraForward);
	updateMatrix();
}

glm::vec3 Engine::Camera::unprojection(glm::vec2 point, float depth) const {
	glm::mat4 invProjView = glm::inverse(projView);
	glm::vec4 worldPosition = invProjView * glm::vec4(point, depth, 1.0f);
	worldPosition /= worldPosition.w;
	return glm::vec3(worldPosition);
}

glm::vec2 Engine::Camera::projection(glm::vec3 position) const {
	glm::vec4 projPosition = projView * glm::vec4(position, 1.0f);
	projPosition /= projPosition.w;
	return glm::vec3(projPosition);
}
