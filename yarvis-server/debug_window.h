
#pragma once

#include "GLFW\glfw3.h"


class DebugWindow
{
private:
	GLFWwindow* window;
	bool running, error, kill;
public:
	DebugWindow() : error(false), running(false), kill(false) {}
	~DebugWindow();

	void operator() ();
	void stop();

	bool isRunning();
	bool hasError();
};
