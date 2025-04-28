#pragma once
// define GLFW_INCLUDE_VULKAN macro to include vulkan header file that was included in glfw
// define the macro before including glfw

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>

#include <iostream>
#include <stdio.h>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <vulkan\vk_enum_string_helper.h>
#include <optional>
#include <set>
#include <filesystem>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>

inline constexpr uint32_t width = 1280u, height = 720u;

#ifdef NDEBUG // If not in debug mode.
const bool enableValidationLayers{ false };
#else // in debug mode
const bool enableValidationLayers{ true };
#endif

// Define Macro helpers
namespace fs = std::filesystem;
#define VK_EXCEPT_MACRO(vk)	 if(vk != VK_SUCCESS)	throw std::runtime_error("Failed to create instance\n")

#define VK_EXCEPT(vk)					\
do {									\
	VkResult result = vk;				\
	std::string to_string = string_VkResult(result);\
													\
	if (result != VK_SUCCESS)						\
	{												\
		std::string error = "Detected Vulkan error { " + to_string + " }"; \
		throw  std::runtime_error(error);			\
	}												\
}while(0)

enum DevInfo
{
	NVIDIA = 0x10DE,
	OTHERS = 0x0000
};

struct ThreadGuard
{
	std::thread m_thread;
	ThreadGuard(std::thread t) :m_thread{ std::move(t) }
	{
		// If there is no available threads then throw an error
		if (!m_thread.joinable())
		{
			throw std::logic_error("No available thread!!\n");
		}
	}

	~ThreadGuard()
	{
		std::clog << "Thread destructor called\n";
		m_thread.join();
	}

};

extern struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

extern struct QueueFamiliyIndicies
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;
	bool IsComplete()const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

// !Function for loading the vkCreateDebugUtilsMessengerEXT extension function.
extern VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

// !Function for loading the vkDestroyDebugUtilsMessengerEXT extension function.
extern void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessanger,
	const VkAllocationCallbacks* pAllocator);

// !Function for loading the vkSubmitDebugUtilsMessageEXT extension function.
extern void SubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);


//! ASSERTONS AND Debugging!!

static void ReportAssertionFailure(const char* expr, const char* file, int line) {
	printf("Assertion failed: %s\nFile: %s\nLine: %d\n", expr, file, line);
	abort();
}

#define ASSERTIONS_ENABLED  1

#if ASSERTIONS_ENABLED
#if defined(_MSC_VER) && !defined(__clang__)
#define _debugBreak() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define _debugBreak() __builtin_trap()
#else
#define _debugBreak() (void)0
#endif

#define ASSERT(expr) \
        do { \
            if (!(expr)) { \
                ReportAssertionFailure(#expr, __FILE__, __LINE__); \
                _debugBreak(); \
            } \
        } while (0)
#else
#define ASSERT(expr) (void)0
#endif