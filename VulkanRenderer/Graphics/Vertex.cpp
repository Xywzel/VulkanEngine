#include "Vertex.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindDescription;
	bindDescription.binding = 0;
	bindDescription.stride = sizeof(Vertex);
	bindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Other option is _INSTANCE for instanced rendering
	return bindDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> descriptions;

	descriptions[0].binding = 0;
	descriptions[0].location = 0;
	descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptions[0].offset = offsetof(Vertex, position);

	descriptions[1].binding = 0;
	descriptions[1].location = 1;
	descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptions[1].offset = offsetof(Vertex, normal);

	descriptions[2].binding = 0;
	descriptions[2].location = 2;
	descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptions[2].offset = offsetof(Vertex, color);

	return descriptions;
}
