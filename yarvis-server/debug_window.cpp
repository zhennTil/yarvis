
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
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "YARVIS Server", NULL, NULL);
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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBegin(GL_QUADS);
		glColor3f(.7f, .7f, .7f);

		// Draw FFT
		float binX, binY, binWidth = 2.f / (FFT_CHUNK_SIZE / 4);
		for (size_t bin = 0; bin < FFT_CHUNK_SIZE / 4; ++bin)
		{
			binX = binWidth * bin - 1.f;
			binY = fft[bin] * 2 - 1;
			glVertex3f(binX, -1.f, 0.f);
			glVertex3f(binX + binWidth - binWidth / 10, -1.f, 0.f);
			glVertex3f(binX + binWidth - binWidth / 10, binY, 0.f);
			glVertex3f(binX, binY, 0.f);
		}

		glEnd();

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

