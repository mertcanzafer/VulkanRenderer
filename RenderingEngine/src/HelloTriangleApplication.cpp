#include "HelloTriangleApplication.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cassert>

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

// Function for loading the vkCreateDebugUtilsMessengerEXT extension function.
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
											const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger);

// Function for loading the vkDestroyDebugUtilsMessengerEXT extension function.
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessanger,
										const VkAllocationCallbacks * pAllocator);

// Function for loading the vkSubmitDebugUtilsMessageEXT extension function.
static void SubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, VkDebugUtilsMessengerCallbackDataEXT * pCallbackData);

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
	SetupDebugMessenger();
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
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_Dmessenger, nullptr);
	}
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
	// Check if all validation layers are supported
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

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

;	auto extensions = GetRequiredExtensions();
	instInfo.enabledExtensionCount = static_cast<uint32_t> (extensions.size());
	instInfo.ppEnabledExtensionNames = extensions.data();

	// Re-use DebugMessengerCreateInfo to be able to use it during VinstanceCreate and VinstanceDestroy
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

	// Modify the instInfo struct
	if (enableValidationLayers)
	{
		instInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		instInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		debugCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		instInfo.enabledLayerCount = 0u;
		instInfo.pNext = nullptr;
	}

	// Creating the instance with given info
	VK_EXCEPT_MACRO( vkCreateInstance(&instInfo, nullptr, &m_Instance) );
}

bool HelloTriangleApplication::CheckValidationLayerSupport()
{
	uint32_t layerCount{ 0u };

	VK_EXCEPT( vkEnumerateInstanceLayerProperties(&layerCount, nullptr) );
	
	std::vector<VkLayerProperties> availableLayers(layerCount);
	VK_EXCEPT( vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()) );
	/*
	* Check if all check if all of the layers in validationLayers exist in the availableLayers list.
	* if not return false
	*/

	for (const auto& layer : this->m_ValidationLayers)
	{
		bool layerFound = false;
		// Search layers inside the availableLayer

		for (const auto& aLayer : availableLayers)
		{
			if (strcmp(layer, aLayer.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}

std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount{ 0u };
	const char** ppGlfwExtensions = nullptr;
	ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(ppGlfwExtensions, ppGlfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::
DebugCallback
(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData
)
{
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void HelloTriangleApplication::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT msgCreateInfo = {};

	// Populate MessengerCreateInfo
	PopulateDebugMessengerCreateInfo(msgCreateInfo);

	VK_EXCEPT( CreateDebugUtilsMessengerEXT(m_Instance,&msgCreateInfo,nullptr,&m_Dmessenger) );
}

void HelloTriangleApplication::PickPhysicalDevice()
{
	EnumeratePhysicalDevices();

	VkPhysicalDeviceProperties deviceProperties = {};
	vkGetPhysicalDeviceProperties(m_device, &deviceProperties);
}

void HelloTriangleApplication::EnumeratePhysicalDevices()
{
	uint32_t physDevCount{ 0u };
	
	assert("Instance must be a valid VkInstance handle" && m_Instance);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, nullptr) );

	assert(physDevCount >= 1);

	std::vector<VkPhysicalDevice> devices(physDevCount);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, devices.data()) );

	// Check If all devices are suitable
	for (auto &device : devices)
	{
		if (IsDeviceValid(device))
		{
			m_device = device;
			break;
		}
	}
	if (m_device == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU!!");
}

inline bool HelloTriangleApplication::IsDeviceValid(VkPhysicalDevice& device)
{
	VkPhysicalDeviceFeatures deviceFeatures = {};
	VkPhysicalDeviceProperties deviceProperties = {};

	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && deviceFeatures.geometryShader;
}

VkResult CreateDebugUtilsMessengerEXT
(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
)
{
	// - vkGetInstanceProcAddr will return nullptr, if the function couldn't be loaded
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT
(
	VkInstance instance, VkDebugUtilsMessengerEXT debugMessanger, const VkAllocationCallbacks* pAllocator
)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
	{
		func(instance, debugMessanger, pAllocator);
	}
	return;
}

void SubmitDebugUtilsMessageEXT
(
	VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{

	auto func =(PFN_vkSubmitDebugUtilsMessageEXT) vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");

	if (func)
		func(instance, messageSeverity, messageType, pCallbackData);
	return;
}

void HelloTriangleApplication:: PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DbgCrtInfo)
{
	DbgCrtInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DbgCrtInfo.pNext = nullptr;
	DbgCrtInfo.flags = 0;
	DbgCrtInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	DbgCrtInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	DbgCrtInfo.pfnUserCallback = DebugCallback;
	DbgCrtInfo.pUserData = nullptr;
}
