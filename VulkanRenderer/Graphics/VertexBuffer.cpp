#include "VertexBuffer.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>
#include "Graphics/Vertex.h"
#include "Graphics/CommandPool.h"


static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	std::cout << "Failed to find suitable memory type" << std::endl;
	return ~0u;
}

VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, size_t size_, size_t count_)
	: device(device)
	, size(size_ * count_)
	, count(count_)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &handle) != VK_SUCCESS)
		std::cout << "Failed to create vertex buffer" << std::endl;
	
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, handle, &memRequirements);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memRequirements.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);
	
	if (vkAllocateMemory(device, &allocateInfo, nullptr, &memory) != VK_SUCCESS)
	{
		std::cout << "failed to allocate vertex buffer memory!" << std::endl;
	}

	vkBindBufferMemory(device, handle, memory, 0);
}

VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(device, handle, nullptr);
	vkFreeMemory(device, memory, nullptr);
}

void VertexBuffer::setData(std::vector<Vertex> vertices)
{
	void *data;
	vkMapMemory(device, memory, 0, size, 0, &data);
	memcpy(data, vertices.data(), size);
	vkUnmapMemory(device, memory);
}

void VertexBuffer::setData(std::vector<uint16_t> indices)
{
	void *data;
	vkMapMemory(device, memory, 0, size, 0, &data);
	memcpy(data, indices.data(), size);
	vkUnmapMemory(device, memory);
}

void VertexBuffer::copyTo(VertexBuffer & other, CommandPool & commandPool, VkQueue queue)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool.getPointer();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, handle, other.handle, 1, &copyRegion);
	
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
	vkFreeCommandBuffers(device, commandPool.getPointer(), 1, &commandBuffer);
}

VkBuffer VertexBuffer::getHandle()
{
	return handle;
}

size_t VertexBuffer::getSize()
{
	return size;
}

size_t VertexBuffer::getCount()
{
	return count;
}
