#include "Application.h"

#include <iostream>

#include "Options.h"
#include "Window.h"
#include "Vulkan.h"

Application::Application()
{
	window = new Window();
	vulkan = new Vulkan(*window);
}

Application::~Application()
{
	if (vulkan)
		delete vulkan;
	if (window)
		delete window;
}

void Application::run()
{
	while (!window->shouldClose())
	{
	}
}
