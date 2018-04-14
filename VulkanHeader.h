/////////////////////////////////////////////////////////////////////
//Graphics Init
#ifndef __ANDROID__
//Desktop Init
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define JNIEnv unsigned char
#define jobject int

#else 
//Android Init

#include <unistd.h>
#include <android/log.h>
#include "vulkan_wrapper.h" // Include Vulkan_wrapper and dynamically load symbols.
#include <unistd.h>

#include "vulkan/vk_sdk_platform.h"
#include <vulkan/vulkan.h>


#endif // !ANDROID
/////////////////////////////////////////////////////////////////////
//Math Init
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
/////////////////////////////////////////////////////////////////////
//Image Load Init
#include <gli\gli.hpp>
/////////////////////////////////////////////////////////////////////
//C++ Function
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
/////////////////////////////////////////////////////////////////////
#pragma once

