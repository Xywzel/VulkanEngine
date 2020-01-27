#include "Window.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Options.h"
#include "Vector.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	Window* self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	self->setWindowResized(true);
}

Window::Window()
	: Window(getOptions().window)
{
}

Window::Window(const WindowOptions & options)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, options.resizable ? GLFW_TRUE : GLFW_FALSE);
	window = glfwCreateWindow(options.width, options.height, options.title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::vector<std::string> Window::getExtensions() const
{
	uint32_t count;
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&count);
	std::vector<std::string> result;
	for (uint32_t i = 0; i < count; ++i)
	{
		result.push_back(std::string(extensions[i]));
	}
	return result;
}

void Window::setWindowResized(bool state)
{
	windowResized = state;
}

bool Window::getWindowResized() const
{
	return windowResized;
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::poolEvents()
{
	glfwPollEvents();
}

void Window::update()
{
	poolEvents();
}

Vec2u Window::getResolution() const
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return Vec2u(width, height);
}

GLFWwindow * Window::getPointer() const
{
	return window;
}
