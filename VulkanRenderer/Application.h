#pragma once
class Window;
class Vulkan;

class Application
{
public:
	Application();
	~Application();
	void run();
	void update();
	void render();

private:
	Window* window = nullptr;
	Vulkan* vulkan = nullptr;
};
