#pragma once

#include <string>
#include <vector>

struct WindowOptions
{
	std::string title = "Vulkan";
	int width = 800;
	int height = 600;
	bool resizable = false;
};

struct VulkanOptions
{
	std::string name = "VulkanRender";
	std::string engine = "VulkanEngine";
	int major = 0;
	int minor = 0;
	int patch = 1;
#ifdef BUILD_DEBUG
	bool validate = true;
#else
	bool validate = false;
#endif
	std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};
	std::vector<const char*> deviceExtensions =
	{
		"VK_KHR_swapchain"
	};
	size_t maxFramesInFlight = 2;
};

struct Options
{
	WindowOptions window;
	VulkanOptions vulkan;
};

const Options& getOptions();
