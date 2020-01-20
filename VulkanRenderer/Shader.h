#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <string>

class RenderPass;

class Shader
{
public:
	Shader(VkDevice& device, VkExtent2D& extent, RenderPass& renderpass);
	~Shader();

	VkPipeline getPipeline();

private:
	VkShaderModule createModule(const std::string& filename);
	void createPipeline(RenderPass& renderpass);
	
	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
	VkDevice device;
	VkExtent2D extent;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
};
