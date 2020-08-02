#pragma once

#include "VulkanTypes.h"
#include <vector>

class CommandPool;
class RenderPass;
class Framebuffer;
class Shader;
class VertexBuffer;

class CommandBuffer
{
public:
	CommandBuffer(VkDevice & device, CommandPool & commandPool, RenderPass & renderPass, Framebuffer & framebuffer, VkExtent2D swapchainExtent, Shader & shader, VertexBuffer& vertexBuffer, VertexBuffer& indexBuffer);
	~CommandBuffer();

	std::vector<VkCommandBuffer>& getBuffers();
private:
	std::vector<VkCommandBuffer> commandBuffers;

	VkDevice device = nullptr;
	CommandPool* commandPool = nullptr;
};
