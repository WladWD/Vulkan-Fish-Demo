#include "Camera.h"

#pragma once
namespace Engine {
	class CameraManager {
	public:
		CameraManager();
		~CameraManager();

		void update(float dt);

	};
}

