#pragma once

// A C++ class style wrapper for GLFW window

#include <vector>
#include <string>

struct GLFWwindow;
struct WindowOptions;
struct Vec2u;

class Window
{
public:
	Window();
	Window(const WindowOptions& options);
	~Window();

	bool shouldClose();
	void update();

	Vec2u getResolution() const;
	GLFWwindow* getPointer() const;
	std::vector<std::string> getExtensions() const;

	void setWindowResized(bool state);
	bool getWindowResized() const;

private:
	void poolEvents();
	GLFWwindow* window;
	bool windowResized = false;
};
