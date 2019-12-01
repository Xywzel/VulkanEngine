#pragma once
class Window;
class Vulkan;

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	Window* window = nullptr;
	Vulkan* vulkan = nullptr;
};
