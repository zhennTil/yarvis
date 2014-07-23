
#pragma once

#include "GLFW\glfw3.h"
#include "constants.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

class DebugWindow
{
private:
	GLFWwindow* window;
	bool running, error, kill;
public:
	DebugWindow() : error(false), running(false), kill(false) {}
	~DebugWindow();

	float fft[FFT_CHUNK_SIZE/2];

	void operator() ();
	void stop();

	bool isRunning();
	bool hasError();
};
