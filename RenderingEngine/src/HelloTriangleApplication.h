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
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT	messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT			messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DbgCrtInfo);
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void EnumeratePhysicalDevices();
	inline bool IsDeviceValid(VkPhysicalDevice& device);
private:
	GLFWwindow* m_pWindow = nullptr;

	// Declare the vulkan instance
	VkInstance m_Instance;

	const std::vector<const char*> m_ValidationLayers
	{
		"VK_LAYER_KHRONOS_validation"
	};
	// Debug messager
	VkDebugUtilsMessengerEXT m_Dmessenger;
	VkPhysicalDevice m_device = VK_NULL_HANDLE; // This will be automatically destroyed when the vkInstance Get destroyed!!
};
