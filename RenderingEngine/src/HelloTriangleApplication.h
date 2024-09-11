#pragma once
// define GLFW_INCLUDE_VULKAN macro to include vulkan header file that was included in glfw
// define the macro before including glfw

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

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
private:
	GLFWwindow* m_pWindow = nullptr;

	// Declare the vulkan instance
	VkInstance m_Instance;
};

// Define a Macro helper
#define VK_EXCEPT_MACRO(vk)	 if(vk != VK_SUCCESS)	throw std::runtime_error("Failed to create instance\n")