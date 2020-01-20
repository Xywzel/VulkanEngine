#pragma once

#include "VulkanTypes.h"
#include <vector>

class CommandPool;
class RenderPass;
class Framebuffer;
class Shader;

class CommandBuffer
{
public:
	CommandBuffer(VkDevice & device, CommandPool & commandPool, RenderPass & renderPass, Framebuffer & framebuffer, VkExtent2D swapchainExtent, Shader & shader);
	~CommandBuffer();

	std::vector<VkCommandBuffer>& getBuffers();
private:
	std::vector<VkCommandBuffer> commandBuffers;
};
