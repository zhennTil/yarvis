
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
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Yarvis Server", NULL, NULL);
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

		// Draw FFT
		glBegin(GL_QUADS);
		glColor3f(.7f, .7f, .7f);

		float binX, binY, binWidth = 2.f / (FFT_CHUNK_SIZE / 4);
		for (size_t bin = 0; bin < FFT_CHUNK_SIZE / 4; ++bin)
		{
			binX = binWidth * bin - 1.f;
			binY = fft[bin] * 2 - 1;
			glVertex2f(binX, -1.f);
			glVertex2f(binX + binWidth - binWidth / 10, -1.f);
			glVertex2f(binX + binWidth - binWidth / 10, binY);
			glVertex2f(binX, binY);
		}

		glEnd();

		// Update BPM if quality threshold is met
		if (*qualityPtr >= BPM_QUALITY_THRESHOLD)
			lastGoodBPM = (*bpmPtr);

		// Draw BPM gauge
		float bpmBot = .6f,
			bpmHeight = .38f,
			bpmLeft = .9f,
			bpmWidth = .08f,
			bpmFrac = (fmaxf((float)lastGoodBPM, 1000.f) / 10.f - 100.f) / 100.f;
		glBegin(GL_LINES);
		glColor3f(.0f, .0f, .6f);

		for (float tick = bpmBot; tick <= bpmBot + bpmHeight; tick += bpmHeight / 10)
		{
			glVertex2f(bpmLeft, tick);
			glVertex2f(bpmLeft-.02f, tick);
		}
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(.0f, .0f, 1.f);

		glVertex2f(bpmLeft, bpmBot);
		glVertex2f(bpmLeft + bpmWidth, bpmBot);
		glVertex2f(bpmLeft + bpmWidth, bpmBot + bpmHeight * bpmFrac);
		glVertex2f(bpmLeft, bpmBot + bpmHeight * bpmFrac);
		glEnd();
		
		// Draw beat lamp
		if ((*beatPtr) % 2 == 0 && (*qualityPtr) >= BEAT_COUNT_QUALITY_THRESHOLD)
		{
			glBegin(GL_QUADS);
			glColor3f(.7f, .0f, .0f);
			glVertex2f(bpmLeft - bpmWidth, 1.f);
			glVertex2f(bpmLeft - .05f, 1.f);
			glVertex2f(bpmLeft - .05f, .9f);
			glVertex2f(bpmLeft - bpmWidth, .9f);
			glEnd();
		}

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

