#pragma once
// define GLFW_INCLUDE_VULKAN macro to include vulkan header file that was included in glfw
// define the macro before including glfw

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <vulkan\vk_enum_string_helper.h>
#include <optional>
#include <set>

const uint32_t width = 1280u, height = 720u;

#ifdef NDEBUG // If not in debug mode.
const bool enableValidationLayers{ false };
#else // in debug mode
const bool enableValidationLayers{ true };
#endif

// Define Macro helpers
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

static struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

static struct QueueFamiliyIndicies
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;
	bool IsComplete()const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

extern enum DevInfo
{
	NVIDIA = 0x00,
	AMD = 0x01
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
