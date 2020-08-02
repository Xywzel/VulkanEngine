#pragma once

#include <array>

#include "glm/glm.hpp"

#include "VulkanTypes.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};
