#include "HelloTriangleApplication.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void HelloTriangleApplication::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	CleanUp();
}

// Init GLFW!!!!
void HelloTriangleApplication::InitWindow()
{
	if (!glfwInit())
	{	
		glfwTerminate();
		throw std::exception("GLFW initialised failed!!!\n");
	}
	// Disable OpenGL for GLFW
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// Disable resizable window feature. Resizing a window is recreating a sum stuff!
	// We are gonna look at this later on!!1
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// Create the window
	m_pWindow = glfwCreateWindow(width, height, "Vulkan App", nullptr, nullptr);
	// Change the window icon Here!!!
	ChangeWindowIcon();
}

void HelloTriangleApplication::InitVulkan()
{
	CreateVinstance();
}

void HelloTriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_pWindow))
	{
		glfwPollEvents();
	}
}

void HelloTriangleApplication::CleanUp()
{
	vkDestroyInstance(m_Instance, nullptr);

	if (m_pWindow)
		glfwDestroyWindow(m_pWindow);
	glfwTerminate();
	m_pWindow = nullptr;
}

void HelloTriangleApplication::ChangeWindowIcon()
{
	// Change the window icon
	int32_t t_width{ 0 }, t_height{ 0 };
	int32_t channels{ 0 };// number of colors comp. Example: RGB has 3 color components.

	unsigned char* pixels = stbi_load("../../icon/icon.jpg", &t_width, &t_height, &channels, 4);

	GLFWimage images[1] = {};
	images[0].width = t_width;
	images[0].height = t_height;
	images[0].pixels = pixels;

	glfwSetWindowIcon(m_pWindow, 1, images);
	stbi_image_free(pixels);
}

void HelloTriangleApplication::CreateVinstance()
{
	// Fill the application info struct!!!
	VkApplicationInfo appInfo = {};
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION( 1,0,0 );
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;

	// Fill the InstanceCreateInfo struct to tell Vk driver ...- 
	// where to use which valdation layers and extensions will be used.
	VkInstanceCreateInfo instInfo = {};
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0u;
	const char** ppGlfwExtensions = nullptr;

	ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	instInfo.enabledExtensionCount = glfwExtensionCount;
	instInfo.ppEnabledExtensionNames = ppGlfwExtensions;

	// Last 2 members of the struct specify the global validation layers to enable
	instInfo.enabledLayerCount = 0u;
	instInfo.pNext = nullptr;

	// Creating the instance with given info
	VK_EXCEPT_MACRO( vkCreateInstance(&instInfo, nullptr, &m_Instance) );
}

bool HelloTriangleApplication::CheckValidationLayerSupport()
{
	uint32_t layerCount{ 0u };

	VK_EXCEPT( vkEnumerateInstanceLayerProperties(&layerCount, nullptr) );

	// TODO Go through all LunarG validation layers support
}
