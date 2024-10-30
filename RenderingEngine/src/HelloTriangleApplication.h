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
#include<optional>

const uint32_t width = 1280u, height = 720u;

class HelloTriangleApplication
{
public:
	void Run();
	HelloTriangleApplication() = default;
	~HelloTriangleApplication() = default;

	HelloTriangleApplication(const HelloTriangleApplication& copy) = delete;
	HelloTriangleApplication& operator =(const HelloTriangleApplication& copy) = delete;
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
	VkInstance m_Instance = VK_NULL_HANDLE;

	const std::vector<const char*> m_ValidationLayers
	{
		"VK_LAYER_KHRONOS_validation"
	};
	// Debug messager
	VkDebugUtilsMessengerEXT m_Dmessenger;
	VkPhysicalDevice m_device = VK_NULL_HANDLE; // This will be automatically destroyed when the vkInstance Get destroyed!!

	struct QueueFamiliyIndicies
	{
		std::optional<uint32_t> GraphicsFamily;
		bool IsComplete()const { return GraphicsFamily.has_value(); }
	};
	QueueFamiliyIndicies m_indicies;

	// Logical Device 
	VkDevice m_Ldevice;
	void CreateLogicalDevice();
private:
	void FindQueueFamilies(VkPhysicalDevice device);
	QueueFamiliyIndicies& getFamiliyIndicies(VkPhysicalDevice device) { FindQueueFamilies(device); return m_indicies; }

	VkQueue m_graphicsQueue;
};



//! Use this mark to demonstrate something important!
//? Use this mark to write questions while writing comments
// Todo write "Todo" for what to do on the code.
//x use x or X for crossing comments out
