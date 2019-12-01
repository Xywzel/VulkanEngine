#pragma once

// A C++ class style wrapper for GLFW window

#include <vector>
#include <string>

struct GLFWwindow;
struct WindowOptions;

class Window
{
public:
	Window();
	Window(const WindowOptions& options);
	~Window();

	std::vector<std::string> getExtensions() const;
	bool shouldClose();

	GLFWwindow* getPointer() const;
private:
	GLFWwindow* window;
};
