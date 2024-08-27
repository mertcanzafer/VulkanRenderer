// define GLFW_INCLUDE_VULKAN macro to include vulkan header file that was included in glfw
// define the macro before including glfw

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>

// Define two more macros to enable 
#define GLM_FORCE_RADIANS // Force all angles being in radians for maths operations
#define	GLM_FORCE_DEPTH_ZERO_TO_ONE // depth buffer range to (0,1) not (-1,1)!

#include <glm.hpp>
#include <mat4x4.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

int main()
{
	constexpr int32_t width{ 1280 }, height{ 720 };

	// Init glfw
	if (!glfwInit())
	{
		std::cerr << "GLFW initialised failed!!!\n" << std::flush;
		glfwTerminate();
		return -1;
	}
	glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);

	GLFWwindow* pWindow = glfwCreateWindow(width, height, "Vulkan App", nullptr, nullptr);

	if (!pWindow)
	{
		std::cerr << "Failed to create the window!!\n" << std::flush;
		glfwTerminate();
		return -1;
	}

	// Change the window icon
	
	int32_t t_width{ 0 }, t_height{ 0 };
	int32_t channels{ 0 };// number of colors comp. Example: RGB has 3 color components.
	unsigned char* pixels = stbi_load("../../icon/icon.jpg",&t_width,&t_height,&channels,4);

	GLFWimage images[1] = {};
	images[0].width = t_width;
	images[0].height = t_height;
	images[0].pixels = pixels;

	glfwSetWindowIcon(pWindow, 1,images);
	stbi_image_free(pixels);

	// Vulkan extension check code
	uint32_t extensionCount{ 0 };
	// Getting the amount for no particular layer and not storing any properties anywhere!
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr); 
	std::cout << "Extension count { " << extensionCount << " }\n";

	// Check out glm functions working conditions
	glm::mat4 matrix(1.0f);
	glm::vec4 vector(-2.0f, -1.0f, 0.0f, 1.0f);

	auto res = matrix * vector;

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents(); // Check the events clikced, pressed, released
	}

	glfwDestroyWindow(pWindow);
	glfwTerminate();
	return 0;
}