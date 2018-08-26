#include "Camera.h"

#pragma once
namespace Engine {
	class CameraManager {
		std::shared_ptr<Camera> camera;
		float moveSpeed;

	public:
		CameraManager(const std::shared_ptr<Camera> &camera, float moveSpeed = 10.0f);
		~CameraManager() = default;

		void moveForward(void);
		void rotateUp(float angle);
		void roatateRight(float angle);
	};
}

