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
		update();
		render();
	}
}

void Application::update()
{
	window->update();
	// Gather input
	// Get delta time
	// Get new state frame
}

void Application::render()
{
	// Get render data from state frame
	// draw frame
	vulkan->drawFrame();
}
