
#pragma once

#include "GLFW\glfw3.h"
#include "constants.h"
#include <cmath>
#include <iostream>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

class DebugWindow
{
private:
	GLFWwindow* window;
	bool running, error, kill;
	int lastGoodBPM;
public:
	DebugWindow() : error(false), running(false), kill(false), lastGoodBPM(0) {}
	~DebugWindow();

	float fft[FFT_CHUNK_SIZE/2];
	int *bpmPtr;
	int *beatPtr;
	float *qualityPtr;

	void operator() ();
	void stop();

	bool isRunning();
	bool hasError();
};
