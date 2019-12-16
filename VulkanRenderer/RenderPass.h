#pragma once

#include "VulkanTypes.h"

class RenderPass
{
public:
	RenderPass(VkDevice device, VkFormat swapchainImageFormat);
	~RenderPass();
	VkRenderPass getPointer();

private:
	VkRenderPass renderpass;
	VkDevice device;
};
