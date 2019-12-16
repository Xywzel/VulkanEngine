#pragma once

#include "VulkanTypes.h"

struct QueueFamilyIndices;

class CommandPool
{
public:
	CommandPool(QueueFamilyIndices queueIndices, VkDevice& device);
	~CommandPool();
private:
	VkCommandPool commandPool;
	VkDevice device;
};
