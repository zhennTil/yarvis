
#include "debug_window.h"

DebugWindow::~DebugWindow()
{
	glfwTerminate();
}

void DebugWindow::operator() ()
{
	running = true;

	// Initialize the library
	if (!glfwInit())
	{
		error = true;
		return;
	}

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		error = true;
		return;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window) && !kill)
	{
		// Render here
		// TODO!

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();

	running = false;
}

void DebugWindow::stop()
{
	kill = true;
}


bool DebugWindow::isRunning()
{
	return running;
}

bool DebugWindow::hasError()
{
	return error;
}

