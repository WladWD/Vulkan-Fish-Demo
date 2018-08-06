#include "VulkanHeader.h"

#pragma once
namespace Engine {
	class Camera {
		float fov;
		float aspectRatio;
		float near;
		float far;
		
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 projView;

		glm::vec3 cameraPosition;
		glm::vec3 cameraForward;
		glm::vec3 cameraUp;
		glm::vec3 cameraRight;

		void updateMatrix(void);
	public:
		Camera(float fov, float aspectRatio, float near = 1.0f, float far = 1000.0f);
		~Camera() = default;

		void resize(float aspectRatio);

		float getFov(void) const;
		float getAspectRatio(void) const;
		float getFarPlane(void) const;
		float getNearPlane(void) const;

		glm::vec3 getCameraWorldPosition(void) const;
		glm::vec3 getCameraForward(void) const;

		glm::mat4 getViewMatrix(void) const;
		glm::mat4 getProjMatrix(void) const;
		glm::mat4 getProjViewMatrix(void) const;

		void setCameraPosition(const glm::vec3 &position);
		void setCameraDirection(const glm::vec3 &forward, const glm::vec3 &up);

		glm::vec3 unprojection(glm::vec2 point) const = delete; //TODO add method
		glm::vec2 projection(glm::vec3 position) const = delete; //TODO add method
	};
}

