#pragma once

// Wrapper for Vulkan Api

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct VulkanOptions;
struct QueueFamilyIndices;
class Window;

class Vulkan
{
public:
	Vulkan(const Window& window);
	Vulkan(const VulkanOptions& options, const Window& window);
	~Vulkan();

private:
	void setupInstance(const VulkanOptions& options, const Window& window);
	void setupMessager(const VulkanOptions& options);
	void setupSurface(const Window& window);
	void setupDevice();
	void setupLogicalDevice(const VulkanOptions& options);
	void setupSwapChain();
	void setupImageViews();

	bool validationLayersSupported(const std::vector<const char*>& validationLayers) const;
	bool suitableDevice(const VkPhysicalDevice& device) const;
	QueueFamilyIndices queueFamily(const VkPhysicalDevice& device) const;
	bool deviceSupportsExtensions(const VkPhysicalDevice& device) const;

	// Members
	VkInstance instance = nullptr;
	VkDebugUtilsMessengerEXT debugMessenger = nullptr;
	VkSurfaceKHR surface = nullptr;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = nullptr;
	VkQueue graphicsQueue = nullptr;
	VkQueue presentationQueue = nullptr;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainFormat;
	VkExtent2D swapchainExtend;
	std::vector<VkImageView> swapchainImageViews;
};