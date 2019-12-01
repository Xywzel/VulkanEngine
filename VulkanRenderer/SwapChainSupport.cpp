#include "SwapChainSupport.h"

#include <vector>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Options.h"

SwapChainSupport::SwapChainSupport(const VkPhysicalDevice & device, const VkSurfaceKHR & surface)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);


	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
	}
	
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		presentationModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentationModes.data());
	}
}

bool SwapChainSupport::adequate() const
{
	return !formats.empty() && !presentationModes.empty();
}

VkSurfaceFormatKHR SwapChainSupport::getBestSurfaceFormat() const
{
	for (const VkSurfaceFormatKHR& format : formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}
	return formats[0];
}

VkPresentModeKHR SwapChainSupport::getBestPresentationMode() const
{
	for (const VkPresentModeKHR& mode : presentationModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChainSupport::getSwapExtent() const
{
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;

	const WindowOptions& options = getOptions().window;
	VkExtent2D actualExtent = {options.width, options.height};
	actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
	return actualExtent;
}
