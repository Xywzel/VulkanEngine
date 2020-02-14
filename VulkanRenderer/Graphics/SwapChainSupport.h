#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class Window;

struct SwapChainSupport
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentationModes;

	SwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
	bool adequate() const;
	VkSurfaceFormatKHR getBestSurfaceFormat() const;
	VkPresentModeKHR getBestPresentationMode() const;
	VkExtent2D getSwapExtent(const Window& window) const;
};
