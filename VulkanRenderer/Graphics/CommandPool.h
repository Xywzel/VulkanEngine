#pragma once

#include "VulkanTypes.h"

struct QueueFamilyIndices;

class CommandPool
{
public:
	CommandPool(QueueFamilyIndices queueIndices, VkDevice& device);
	~CommandPool();
	VkCommandPool getPointer();
private:
	VkCommandPool commandPool;
	VkDevice device;
};
