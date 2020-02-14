#pragma once

#include <vector>

#include "VulkanTypes.h"

class RenderPass;

class Framebuffer
{
public:
	Framebuffer(RenderPass& renderpass, std::vector<VkImageView> swapChainImages, VkDevice& device, VkExtent2D& extent);
	~Framebuffer();
	std::vector<VkFramebuffer>& getBuffers();
private:
	VkDevice device = nullptr;
	std::vector<VkFramebuffer> buffers;
};
