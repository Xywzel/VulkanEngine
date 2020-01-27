#include "CommandBuffer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "CommandPool.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "Shader.h"

#include <iostream>

CommandBuffer::CommandBuffer(VkDevice& device, CommandPool& commandPool_, RenderPass& renderPass, Framebuffer& framebuffer, VkExtent2D swapchainExtent, Shader& shader)
	: device(device)
{
	commandPool = &commandPool_;
	commandBuffers.resize(size_t(framebuffer.getBuffers().size()));
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool_.getPointer();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = uint32_t(commandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		std::cout << "failed to allocate command buffers!" << std::endl;

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			std::cout << "failed to begin recording command buffer!" << std::endl;
	
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.getPointer();
		renderPassInfo.framebuffer = framebuffer.getBuffers()[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapchainExtent;

		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, shader.getPipeline());
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			std::cout << "failed to record command buffer!" << std::endl;
	}
}

CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(device, commandPool->getPointer(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

std::vector<VkCommandBuffer>& CommandBuffer::getBuffers()
{
	return commandBuffers;
}
