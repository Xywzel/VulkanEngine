#include "CommandPool.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <iostream>

#include "QueueFamilyIndices.h"

CommandPool::CommandPool(QueueFamilyIndices queueIndices, VkDevice & device)
	: device(device)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueIndices.graphics.value();
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		std::cout << "failed to create command pool!" << std::endl;
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
}

VkCommandPool CommandPool::getPointer()
{
	return commandPool;
}
