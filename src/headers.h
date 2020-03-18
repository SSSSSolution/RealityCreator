#ifndef HEADERS_H
#define HEADERS_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define APP_NAME_STR_LEN 80
#define _CRT_SECURE_NO_WARNINGS
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif // _WIN32


/********** C/C++ HEADERS FILES *********/
#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <assert.h>

#include <memory>
#include <mutex>

/*********** VULKAN HEADER FILES **********/
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <QDebug>

#endif // HEADERS_H
