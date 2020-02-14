#include "Vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <set>
#include <algorithm>

#include "Options.h"
#include "Window.h"
#include "QueueFamilyIndices.h"
#include "SwapChainSupport.h"
#include "Shader.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Math/Vector.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		// Message is important enough to show
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	}
    return VK_FALSE;
}

VkResult createDebugMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, nullptr, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, nullptr);
	}
}

Vulkan::Vulkan(const Window& window)
	: Vulkan(getOptions().vulkan, window)
{
}

Vulkan::Vulkan(const VulkanOptions& options, const Window& window)
{
	setupInstance(options, window);
	setupMessager(options);
	setupSurface(window);
	setupDevice();
	setupLogicalDevice(options);
	setupSwapChain(window);
	setupImageViews();
	renderpass = new RenderPass(device, swapchainFormat);
	shader = new Shader(device, swapchainExtent, *renderpass);
	commandPool = new CommandPool(queueFamily(physicalDevice), device);

	framebuffer = new Framebuffer(*renderpass, swapchainImageViews, device, swapchainExtent);

	commandBuffer = new CommandBuffer(device, *commandPool, *renderpass, *framebuffer, swapchainExtent, *shader);
	setupSemaphores(options);
}

Vulkan::~Vulkan()
{
	vkDeviceWaitIdle(device);
	for (VkSemaphore semaphore : renderFinishedSemaphores)
		vkDestroySemaphore(device, semaphore, nullptr);
	for (VkSemaphore semaphore : imageAvailableSemaphores)
		vkDestroySemaphore(device, semaphore, nullptr);
	for (VkFence fence : inFlightFences)
		vkDestroyFence(device, fence, nullptr);
	if (framebuffer)
		delete framebuffer;
	if (commandBuffer)
		delete commandBuffer;
	if (shader)
		delete shader;
	if (renderpass)
		delete renderpass;
	for (VkImageView& view : swapchainImageViews)
		vkDestroyImageView(device, view, nullptr);
	if (swapchain)
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	if (commandPool)
		delete commandPool;
	if (device)
		vkDestroyDevice(device, nullptr);
	if (surface)
		vkDestroySurfaceKHR(instance, surface, nullptr);
	if (debugMessenger)
		destroyDebugMessenger(instance, debugMessenger);
	if (instance)
		vkDestroyInstance(instance, nullptr);
}

void Vulkan::drawFrame(const Window & window)
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain(window);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		std::cout << "Failed to acquire swap chain" << std::endl;
		return;
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	// Mark the image as now being in use by this frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer->getBuffers()[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		std::cout << "failed to submit draw command buffer!" << std::endl;

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(presentationQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.getWindowResized())
	{
		recreateSwapchain(window);
		return;
	}
	else if (result != VK_SUCCESS)
	{
		std::cout << "Failed to acquire swap chain" << std::endl;
		return;
	}

	currentFrame = (currentFrame + 1) % frameCount;
}

void Vulkan::setupInstance(const VulkanOptions& options, const Window& window)
{

	if (options.validate && !validationLayersSupported(options.validationLayers))
		std::cout << "Requested validation layers not supported" << std::endl;

	VkApplicationInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = options.name.c_str();
	info.applicationVersion = VK_MAKE_VERSION(options.major, options.minor, options.patch);
	info.pEngineName = options.engine.c_str();
	info.engineVersion = VK_MAKE_VERSION(options.major, options.minor, options.patch);
	info.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &info;

	// Extensions
	std::vector<std::string> names = window.getExtensions();
	if (options.validate)
		names.push_back(std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
	createInfo.enabledExtensionCount = uint32_t(names.size());
	std::vector<char*> cstrings;
    cstrings.reserve(names.size());
    for(const std::string& name : names)
        cstrings.push_back(const_cast<char*>(name.c_str()));
	createInfo.ppEnabledExtensionNames = cstrings.data();

	// Validation layers
	if (options.validate)
	{
		createInfo.enabledLayerCount = options.validationLayers.size();
		createInfo.ppEnabledLayerNames = options.validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
		std::cout << "Failed to create instance" << std::endl;
}

void Vulkan::setupMessager(const VulkanOptions& options)
{
	if (!options.validate)
		return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	VkResult result = createDebugMessenger(instance, &createInfo, &debugMessenger);
	if (result != VK_SUCCESS)
		std::cout << "Failed to create debuger" << std::endl;
}

void Vulkan::setupSurface(const Window & window)
{
	VkResult result = glfwCreateWindowSurface(instance, window.getPointer(), nullptr, &surface);
	if (result != VK_SUCCESS)
		std::cout << "surface greation failed" << std::endl;
}

void Vulkan::setupDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		std::cout << "No Vulkan devices found" << std::endl;
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const VkPhysicalDevice& device : devices)
	{
		if (suitableDevice(device))
		{
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE)
		std::cout << "No suitable device found" << std::endl;
}

void Vulkan::setupLogicalDevice(const VulkanOptions& options)
{
	// Queues
	QueueFamilyIndices indices = queueFamily(physicalDevice);	
	std::set<uint32_t> uniqueIndices = {indices.graphics.value(), indices.presentation.value()};

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;
	for (uint32_t index : uniqueIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = index;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Device features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Create logical device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = uint32_t(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	// Logical device extensions
	createInfo.enabledExtensionCount = uint32_t(options.deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = options.deviceExtensions.data();
	// Logical device validation layers for backwards compality
	if (options.validate)
	{
		createInfo.enabledLayerCount = uint32_t(options.validationLayers.size());
		createInfo.ppEnabledLayerNames = options.validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	// Create the device 
	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
		std::cout << "failed to create logical device!" << std::endl;

	// Get Queue handle
	vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentation.value(), 0, &presentationQueue);
}

void Vulkan::setupSwapChain(const Window & window)
{
	SwapChainSupport details(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = details.getBestSurfaceFormat();
	VkPresentModeKHR presentMode = details.getBestPresentationMode();
	VkExtent2D extent = details.getSwapExtent(window);

	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0)
		imageCount = std::min(details.capabilities.maxImageCount, imageCount);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = queueFamily(physicalDevice);
	uint32_t queueIndices[] = {indices.graphics.value(), indices.presentation.value()};
	if (indices.graphics != indices.presentation)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueIndices;
	}
	else
	{
		// This has better performance, should handle transfer explicitly and get rid of concurrent
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = details.capabilities.currentTransform;
	// How to blend with other windows and desktop
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	// This means pixels behind other windows don't need to be rendered
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
		std::cout << "failed to create swapchain!" << std::endl;

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	swapchainExtent = extent;
	swapchainFormat = surfaceFormat.format;
}

void Vulkan::setupImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainImageViews.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			std::cout << "failed to create image views!" << std::endl;
		}
	}
}

void Vulkan::setupSemaphores(const VulkanOptions& options)
{
	frameCount = options.maxFramesInFlight;

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	imageAvailableSemaphores.resize(frameCount);
	renderFinishedSemaphores.resize(frameCount);
	inFlightFences.resize(frameCount);
	imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);


	for (size_t i = 0; i < frameCount; ++i)
	{
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS)
			std::cout << "failed to create semaphores!" << std::endl;
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
			std::cout << "failed to create semaphores!" << std::endl;
		if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			std::cout << "failed to create fences!" << std::endl;
	}
}

bool Vulkan::validationLayersSupported(const std::vector<const char*>& validationLayers) const
{
	// Get supported layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Check if requested layers are supported
	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
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

bool Vulkan::suitableDevice(const VkPhysicalDevice & device) const
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	SwapChainSupport swapChain(device, surface);

	bool suitable = true;
	suitable = suitable && (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
	suitable = suitable && deviceFeatures.geometryShader;
	suitable = suitable && queueFamily(device).isComplete();
	suitable = suitable && deviceSupportsExtensions(device);
	suitable = suitable && swapChain.adequate();

	return suitable;
}

QueueFamilyIndices Vulkan::queueFamily(const VkPhysicalDevice & device) const
{
	QueueFamilyIndices result;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		// Get current family
		const VkQueueFamilyProperties& family = queueFamilies[i];

		// Test for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			result.graphics = i;

		// Test for presentation support
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
		if (presentationSupport)
			result.presentation = i;
	}
	return result;
}

bool Vulkan::deviceSupportsExtensions(const VkPhysicalDevice & device) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(getOptions().vulkan.deviceExtensions.begin(), getOptions().vulkan.deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void Vulkan::recreateSwapchain(const Window & window)
{
	// Don't recreate when minimizing the window, as there is nothing to render
	Vec2u res = window.getResolution();
	if (res.x == 0u || res.y == 0u)
		return;

	vkDeviceWaitIdle(device);
	cleanSwapchain();

	setupSwapChain(window);
	setupImageViews();
	renderpass = new RenderPass(device, swapchainFormat);
	shader = new Shader(device, swapchainExtent, *renderpass);
	framebuffer = new Framebuffer(*renderpass, swapchainImageViews, device, swapchainExtent);
	commandBuffer = new CommandBuffer(device, *commandPool, *renderpass, *framebuffer, swapchainExtent, *shader);

	std::cout << "Swapchain recreated" << std::endl;
}


void Vulkan::cleanSwapchain()
{
	if (commandBuffer)
		delete commandBuffer;
	if (framebuffer)
		delete framebuffer;
	if (shader)
		delete shader;
	if (renderpass)
		delete renderpass;
	for (VkImageView& view : swapchainImageViews)
		vkDestroyImageView(device, view, nullptr);
	if (swapchain)
		vkDestroySwapchainKHR(device, swapchain, nullptr);
}
