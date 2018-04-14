#include "VulkanWindow.h"
#include <iostream>

#define WAIT_TO_USER_READ std::cout << "\tPress [Enter] to close.\n"; \
						  std::cin.get();	
int main() {
	int result = EXIT_SUCCESS;

	try {
		VulkanEngineApplication::VulkanWindow window(0, 0);
		window.initialize(1280, 720);

		while (!window.windowShoudClose()) {
			glfwPollEvents();
			window.draw();
		}

	}
	catch (const std::runtime_error &e) {
		std::cerr << "[ERROR] - " << e.what();
		result = EXIT_FAILURE;
	}

	WAIT_TO_USER_READ
	return result;
}