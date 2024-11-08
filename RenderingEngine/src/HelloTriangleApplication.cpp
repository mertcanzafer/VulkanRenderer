#include "HelloTriangleApplication.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cassert>

static VkPhysicalDeviceFeatures g_deviceFeatures = {};
static VkPhysicalDeviceProperties g_deviceProperties = {};

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
	PickPhysicalDevice();
	CreateLogicalDevice();
}

void HelloTriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_pWindow))
	{
		std::clog << "In the loop!\n";
		glfwPollEvents();
	}
}

void HelloTriangleApplication::CleanUp()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_Instance, m_Dmessenger, nullptr);
	}
	vkDestroyDevice(m_Ldevice, nullptr);
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
	VK_EXCEPT( vkCreateInstance(&instInfo, nullptr, &m_Instance) );
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
	VkPhysicalDeviceProperties2 deviceProperties = {};
	// !=
	deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	deviceProperties.pNext = nullptr;
	//! Validation layer triggerd when calling vkGetPhysicalDeviceProperties2 function
	//Todo Examine the function parameters and implementation
	vkGetPhysicalDeviceProperties2(m_device, &deviceProperties);
}

void HelloTriangleApplication::EnumeratePhysicalDevices()
{
	uint32_t physDevCount{ 0u };
	uint32_t deviceIndex = { 0u }, deviceCounter{0u};
	
	// ! You can switch NVIDIA to AMD by changing the DevInfo enum type
	// ? These lines of code might be changed in the future!!
	enum DevInfo devType = NVIDIA;

	assert("Instance must be a valid VkInstance handle" && m_Instance);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, nullptr) );

	assert(physDevCount != 0 && "Filed to find GPUs with Vulkan Support!!\n");

	std::vector<VkPhysicalDevice> devices(physDevCount);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, devices.data()) );

	switch (IsVendorNVIDIA(devType))
	{
	case 0: // AMD GFX device!
		if(IsDeviceValid(devices[0]))
			m_device = devices[0];
		break;
	case 1: // NVIDIA GFX device
		if(IsDeviceValid(devices[1]))
			m_device = devices[1];
		break;
	}

	if (m_device == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU!!\n");

	vkGetPhysicalDeviceProperties(m_device, &g_deviceProperties);
	std::clog << "Device: " << g_deviceProperties.deviceName << std::endl;

	//// Check If all devices are suitable
	//for (auto &device : devices)
	//{
	//	if (IsDeviceValid(device) && IsVendorNVIDIA(devType))
	//	{
	//		m_device = device;
	//		deviceIndex = deviceCounter;
	//		break;
	//	}
	//	deviceCounter++;
	//}
	//if (m_device == VK_NULL_HANDLE)
	//	throw std::runtime_error("Failed to find a suitable GPU!!");
	//assert(deviceIndex < 2);
	////! Extract the NVIDIA device!!!  -> devices[0] = AMD GFX device, devices[1] = NVIDIA GTX device on my computer.
	////! This part is optional as you might want to choose a specific device for graphics render pipeline.
	////! However, you have to update your all graphics drivers before vulkan layers start to do rendering pixels to the screen
	//m_device = devices.at(static_cast<std::vector<VkPhysicalDevice, std::allocator<VkPhysicalDevice>>::size_type>(deviceIndex) + 1);
	////vkGetPhysicalDeviceProperties(m_device, &g_deviceProperties);
}

inline bool HelloTriangleApplication::IsDeviceValid(VkPhysicalDevice& device)
{
	//? If you are sure that it picks the most suitable device. Then keep the IsDevice valid function like this 
	const QueueFamiliyIndicies indices = getFamiliyIndicies(device);
	return indices.IsComplete();
}

void HelloTriangleApplication::CreateLogicalDevice()
{
	QueueFamiliyIndicies indices = getFamiliyIndicies(m_device);
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
	queueCreateInfo.queueCount = 1u;

	float queuePriority = { 1.0f };
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.pQueueCreateInfos = &queueCreateInfo;
	CreateInfo.queueCreateInfoCount = 1u;
	CreateInfo.pEnabledFeatures = &g_deviceFeatures;
	CreateInfo.enabledExtensionCount = 0u;

	if (enableValidationLayers)
	{
		CreateInfo.enabledLayerCount = (uint32_t)m_ValidationLayers.size();
		CreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
		CreateInfo.enabledLayerCount = 0u;

	VK_EXCEPT( vkCreateDevice(m_device, &CreateInfo, nullptr, &m_Ldevice) );

	vkGetDeviceQueue(m_Ldevice, indices.GraphicsFamily.value(), 0u, &m_graphicsQueue);
}

void HelloTriangleApplication::FindQueueFamilies(VkPhysicalDevice device)
{
	uint32_t queueFamilyCount{ 0u };
	assert("Device must be valid device!!!" && device != VK_NULL_HANDLE);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, properties.data());
	//Todo Look up the Vulkan Spec!!!
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			// This Queue Family support graphics
			m_indicies.GraphicsFamily = i;
			if (m_indicies.GraphicsFamily.has_value())
				break;
		}
	}

	if (!m_indicies.IsComplete())
	{
		throw std::runtime_error("Unvalid graphics queue Family!!\n");
	}
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

bool IsVendorNVIDIA(enum DevInfo& info)
{
	return (info == NVIDIA) ? true : false;
}
