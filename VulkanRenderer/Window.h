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

	bool shouldClose();
	void update();

	GLFWwindow* getPointer() const;
	std::vector<std::string> getExtensions() const;
private:
	void poolEvents();
	GLFWwindow* window;
};
