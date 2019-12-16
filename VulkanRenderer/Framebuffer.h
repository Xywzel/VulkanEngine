#pragma once

#include <vector>

#include "VulkanTypes.h"

class RenderPass;

class Framebuffer
{
public:
	Framebuffer(RenderPass& renderpass, std::vector<VkImageView> attachments, VkDevice& device, VkExtent2D& extent);
	~Framebuffer();
private:
	VkDevice device = nullptr;
	VkFramebuffer buffer = nullptr;
};
