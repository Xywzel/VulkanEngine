#pragma once

// Wrapper for Vulkan Api

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

struct VulkanOptions;
struct QueueFamilyIndices;
class Window;
class Shader;
class RenderPass;
class Framebuffer;
class CommandPool;
class CommandBuffer;

class Vulkan
{
public:
	Vulkan(const Window& window);
	Vulkan(const VulkanOptions& options, const Window& window);
	~Vulkan();

	void drawFrame(const Window& window);

private:
	void setupInstance(const VulkanOptions& options, const Window& window);
	void setupMessager(const VulkanOptions& options);
	void setupSurface(const Window& window);
	void setupDevice();
	void setupLogicalDevice(const VulkanOptions& options);
	void setupSwapChain(const Window& window);
	void setupImageViews();
	void setupSemaphores(const VulkanOptions& options);

	bool validationLayersSupported(const std::vector<const char*>& validationLayers) const;
	bool suitableDevice(const VkPhysicalDevice& device) const;
	QueueFamilyIndices queueFamily(const VkPhysicalDevice& device) const;
	bool deviceSupportsExtensions(const VkPhysicalDevice& device) const;

	void recreateSwapchain(const Window& window);
	void cleanSwapchain();

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
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
	RenderPass* renderpass = nullptr;
	Shader* shader = nullptr;
	Framebuffer* framebuffer = nullptr;
	CommandPool* commandPool = nullptr;
	CommandBuffer* commandBuffer = nullptr;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	size_t currentFrame = 0;
	size_t frameCount = 0;
};
