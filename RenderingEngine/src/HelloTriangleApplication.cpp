#include "HelloTriangleApplication.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Timer.h"

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
	CreateSurface();
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
	vkDestroySurfaceKHR(m_Instance, m_surface, nullptr);
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
	appInfo.apiVersion = VK_API_VERSION_1_1;
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
	ASSERT(m_device != VK_NULL_HANDLE && "The device is not valid!!!\n");

	Timer t;
	vkGetPhysicalDeviceProperties2(m_device, &deviceProperties);
	std::clog << "Took " << t.Elapsed() << " seconds\n";
}

void HelloTriangleApplication::EnumeratePhysicalDevices()
{
	uint32_t physDevCount{ 0u };

	ASSERT("Instance must be a valid VkInstance handle" && m_Instance);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, nullptr) );

	ASSERT(physDevCount != 0 && "Filed to find GPUs with Vulkan Support!!\n");

	std::vector<VkPhysicalDevice> devices(physDevCount);
	VK_EXCEPT( vkEnumeratePhysicalDevices(m_Instance, &physDevCount, devices.data()) );

	// ! You can switch NVIDIA to AMD by changing the DevInfo enum type
	// ? These lines of code might be changed in the future!!
	enum DevInfo devType = NVIDIA;

	for (const auto& device : devices)
	{
		vkGetPhysicalDeviceProperties(device, &g_deviceProperties);

		if (g_deviceProperties.vendorID == devType && IsDeviceValid(device))
		{
			m_device = device;
			break;
		}
	}

	if (m_device == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU!!\n");
	std::clog << "Device: " << g_deviceProperties.deviceName << std::endl;
}

inline bool HelloTriangleApplication::IsDeviceValid(VkPhysicalDevice device)
{
	//? If you are sure that it picks the most suitable device. Then keep the IsDevice valid function like this 
	const QueueFamiliyIndicies indices = getFamiliyIndicies(device);
	bool extensionSupported = CheckDeviceExtension(device);

	bool swapChainAdequate = false;

	if (extensionSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionSupported && swapChainAdequate;
}

void HelloTriangleApplication::CreateLogicalDevice()
{
	QueueFamiliyIndicies indices = getFamiliyIndicies(m_device);

	std::set<uint32_t> uniqueQueueFamilies{ indices.GraphicsFamily.value(),indices.PresentFamily.value() };
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.reserve(uniqueQueueFamilies.size());

	float queuePriority = { 1.0f };
	for (const auto& queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = (uint32_t)queueFamily;
		queueCreateInfo.queueCount = 1u;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	CreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	CreateInfo.pEnabledFeatures = &g_deviceFeatures;
	CreateInfo.enabledExtensionCount = (uint32_t)m_DeviceExtensionList.size();
	CreateInfo.ppEnabledExtensionNames = m_DeviceExtensionList.data();

	if (enableValidationLayers)
	{
		CreateInfo.enabledLayerCount = (uint32_t)m_ValidationLayers.size();
		CreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
		CreateInfo.enabledLayerCount = 0u;

	VK_EXCEPT( vkCreateDevice(m_device, &CreateInfo, nullptr, &m_Ldevice) );

	vkGetDeviceQueue(m_Ldevice, indices.GraphicsFamily.value(), 0u, &m_graphicsQueue);
	vkGetDeviceQueue(m_Ldevice, indices.PresentFamily.value(), 0u, &m_presentQueue);
}

void HelloTriangleApplication::FindQueueFamilies(VkPhysicalDevice device)
{
	uint32_t queueFamilyCount{ 0u };
	ASSERT("Device must be valid device!!!" && device != VK_NULL_HANDLE);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, properties.data());
	//Todo Look up the Vulkan Spec!!!
	VkBool32 presentSupport = false;
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			// This Queue Family support graphics
			m_indicies.GraphicsFamily = i;
		}
		presentSupport = false;
		VK_EXCEPT( vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport) );

		if (presentSupport)
		{
			m_indicies.PresentFamily = i;
		}
		if (m_indicies.IsComplete()) break;
	}

	if (!m_indicies.IsComplete())
	{
		throw std::runtime_error("Unvalid graphics queue Family!!\n");
	}
}

void HelloTriangleApplication::CreateSurface()
{
	ASSERT(m_Instance != VK_NULL_HANDLE && "Invalid instance!!!");
	ASSERT(m_pWindow != nullptr && "Window instance must not be a null ptr!!!!");
	VK_EXCEPT( glfwCreateWindowSurface(m_Instance, m_pWindow, nullptr, &m_surface) );
	// !vkSurface should be destroyed before destroying m_instance!!!
}

bool HelloTriangleApplication::CheckDeviceExtension(VkPhysicalDevice& device) const
{
	uint32_t extensionCount{};

	ASSERT(device != VK_NULL_HANDLE && "Device must be valid!!");
	VK_EXCEPT( vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,nullptr) );

	std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	uint32_t propertiesCount = (uint32_t)extensionProperties.size();
	VK_EXCEPT(
		vkEnumerateDeviceExtensionProperties(device, nullptr, &propertiesCount, extensionProperties.data());
	);

	std::set<std::string> requiredExtensions(m_DeviceExtensionList.begin(), m_DeviceExtensionList.end());

	for (const auto& ext : extensionProperties)
	{
		requiredExtensions.erase(ext.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails HelloTriangleApplication::QuerySwapChainSupport(VkPhysicalDevice& device) const
{
	SwapChainSupportDetails details = {};
	
	ASSERT(m_surface != VK_NULL_HANDLE && device != VK_NULL_HANDLE &&  
		"surface and device must be valid handles");
	// Query capabilties of surface and device.
	VK_EXCEPT(
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
	);
	// Query Format of the surface and the device.
	uint32_t formatCount = {};

	VK_EXCEPT(
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	);

	if (formatCount)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	// Query PresentMode of the surface and the device. 
	uint32_t presentModeCount = {};
	VK_EXCEPT(
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	);

	if (presentModeCount)
	{
		details.formats.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
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
