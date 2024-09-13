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

const uint32_t width = 1280u, height = 720u;

class HelloTriangleApplication
{
public:
	void Run();
	HelloTriangleApplication() = default;
	~HelloTriangleApplication() = default;
private:
	void InitWindow(); // For GLFW window creation
	void InitVulkan();
	void MainLoop();
	void CleanUp();
	void ChangeWindowIcon(); // For Changing window icon!!
	void CreateVinstance();

	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	/*	Debug Callback Func
	*  explained: https://stackoverflow.com/questions/76247390/how-does-the-vulkan-validation-layer-assignment-work
	*<summary>
	* Param: VkDebugUtilsMessageSeverityFlagBitsEXT 
	* - The first parameter specifies the severity of the message, which is one of the following flags:		
	* - VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
	* - VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
	* - VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
	* - VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
	* Param: messageType 
	* - VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
	* - VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
	* - VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
	* Param: pCallbackData 
	* Parameter refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of the message itself, ...-
	* with the most important members being:
	* - pMessage: The debug message as a null-terminated string
	* - pMessage: Array of Vulkan object handles related to the message
	* - objectCount: Number of objects in array
	* Param: pUserData 
	* The parameter contains a pointer that was specified during the setup of the callback and allows you to pass your own data to it.
	*</summary>
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT	messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT			messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

private:
	GLFWwindow* m_pWindow = nullptr;

	// Declare the vulkan instance
	VkInstance m_Instance;

	const std::vector<const char*> m_ValidationLayers
	{
		"VK_LAYER_KHRONOS_validation"
	};
};
