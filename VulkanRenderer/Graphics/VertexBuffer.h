#pragma once

#include <vector>
#include "VulkanTypes.h"

struct Vertex;
struct CommandPool;

class VertexBuffer
{
public:
	VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, size_t size, size_t count);
	~VertexBuffer();

	void setData(std::vector<Vertex> vertices);
	void setData(std::vector<uint16_t> indices);

	void copyTo(VertexBuffer& other, CommandPool& commandPool, VkQueue queue);

	VkBuffer getHandle();
	size_t getSize();
	size_t getCount();

private:
	VkBuffer handle;
	VkDeviceMemory memory;
	VkDevice device;
	size_t size;
	size_t count;
};
