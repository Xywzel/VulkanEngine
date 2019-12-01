#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <string>

class Shader
{
public:
	Shader(VkDevice& device, VkExtent2D& extent);
	~Shader();

private:
	VkShaderModule createModule(const std::string& filename);
	void createPipeline();
	
	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
	VkDevice device;
	VkExtent2D extent;
};
