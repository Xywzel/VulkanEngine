#include "Framebuffer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>
#include "RenderPass.h"


Framebuffer::Framebuffer(RenderPass & renderpass, std::vector<VkImageView> attachments, VkDevice & device, VkExtent2D& extent)
	: device(device)
{
	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderpass.getPointer();
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &buffer) != VK_SUCCESS)
		std::cout << "failed to create framebuffer!" << std::endl;
}

Framebuffer::~Framebuffer()
{
	if (buffer)
		vkDestroyFramebuffer(device, buffer, nullptr);
}
