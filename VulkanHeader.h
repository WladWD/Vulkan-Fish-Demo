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

//#define VK_USE_PLATFORM_ANDROID_KHR


#include <unistd.h>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
//#include <vulkan/vulkan.h>
//#include <vulkan/vk_platform.h>
#include <android_native_app_glue.h>
#include "vulkan_wrapper.h"

#endif // !ANDROID
/////////////////////////////////////////////////////////////////////
//Math Init
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
/////////////////////////////////////////////////////////////////////
//Image Load Init
#include <gli\texture2d.hpp>
#include <gli\convert.hpp>
#include <gli\generate_mipmaps.hpp>
#include <gli\load.hpp>
#include <gli\save.hpp>
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

