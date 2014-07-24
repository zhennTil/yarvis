
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include "portaudio.h"
#include "kiss_fftr.h"
#include "debug_window.h"
#include "audio_analyzer.h"

using namespace std;

#define HANDLE_PA_ERROR(err, situation) if(err!=paNoError){ cerr << "ERROR in PortAudio " << situation << ": " << Pa_GetErrorText(err) << endl; return 1; }

typedef struct
{
	float buffer[AUDIO_BUFFER_SIZE];
	size_t bufferHead;
} PaUserData;

static int processAudioInput(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo *timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	
	PaUserData *ud = (PaUserData*)userData;
	float *in = (float*)inputBuffer;

	for (unsigned long i = 0; i < framesPerBuffer; i++)
	{
		ud->buffer[ud->bufferHead] = in[i];
		if (ud->bufferHead == AUDIO_BUFFER_SIZE - 1)
			ud->bufferHead = 0;
		else
			++ud->bufferHead;
	}
	
	return 0;
}

PaDeviceIndex selectInputDevice()
{
	// TODO: Provide fallback device selector if Pa_GetDefaultInputDevice() fails. Even better, with a cmake option.

	PaDeviceIndex numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) HANDLE_PA_ERROR(numDevices, "device counting");

	vector<PaDeviceIndex> inputDeviceIndices;
	const PaDeviceInfo *deviceInfo;
	for (PaDeviceIndex i = 0; i<numDevices; ++i)
	{
		deviceInfo = Pa_GetDeviceInfo(i);

		if (deviceInfo->maxInputChannels > 0)
			inputDeviceIndices.push_back(i);
	}

	cout << inputDeviceIndices.size() << " input devices found." << endl;
	for (size_t i = 0; i < inputDeviceIndices.size(); ++i)
	{
		deviceInfo = Pa_GetDeviceInfo(inputDeviceIndices[i]);

		cout << "Device #" << i << ": " << deviceInfo->name << endl;
		cout << "\t" << Pa_GetHostApiInfo(deviceInfo->hostApi)->name << endl;
	}

	cout << "Select a device: ";
	size_t selectedI;
	cin >> selectedI;
	
	//cout << Pa_GetHostApiInfo(deviceInfo->hostApi)->name << endl;
	
	return inputDeviceIndices[selectedI];

	//return Pa_GetDefaultInputDevice();
}

PaError openInputStream(PaStream** stream, const PaDeviceIndex& device, PaUserData* data)
{
	PaStreamParameters inputParameters;

	memset(&inputParameters, 0, sizeof(inputParameters));
	inputParameters.channelCount = 1;
	inputParameters.device = device;
	inputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

	cout << Pa_GetDeviceInfo(device)->maxInputChannels << endl;

	return Pa_OpenStream(
		stream,
		&inputParameters,
		NULL,
		44100,
		paFramesPerBufferUnspecified,
		paNoFlag,
		processAudioInput,
		data);
}

int main(int argc, char* argv[])
{
	// Initialize shared resources
	PaUserData data;
	data.bufferHead = 0;

	// Open audio input
	PaError err = Pa_Initialize();
	HANDLE_PA_ERROR(err, "initialization");

	PaDeviceIndex inputIndex = selectInputDevice();

	PaStream *stream;
	err = openInputStream(&stream, inputIndex, &data);
	HANDLE_PA_ERROR(err, "stream open");
	
	err = Pa_StartStream(stream);
	HANDLE_PA_ERROR(err, "stream start");

	// Set up debug window
	// TODO: Commandline and/or cmake toggle
	DebugWindow window;
	thread drawThread([&]() {window(); });
	
	// Set up audio processing thread
	AudioAnalyzer analyzer(data.buffer, data.bufferHead, window.fft);
	thread analyzeThread([&]() {analyzer.loop(); });

	window.bpmPtr = &(analyzer.beat->win_bpm_int);
	window.beatTimerPtr = &(analyzer.beat->bpm_offset);
	window.beatPtr = &(analyzer.beat->beat_counter);
	window.qualityPtr = &(analyzer.beat->quality_avg);

	cout << "Type 'q' and enter to quit." << endl;
	// Check command input
	char cmd = ' ';
	do
	{
		cin >> cmd;
	} while (cmd != 'q');

	analyzer.stop();
	analyzeThread.join();

	window.stop();
	drawThread.join();

	// Terminate audio input
	err = Pa_StopStream(stream);
	HANDLE_PA_ERROR(err, "stream stop");

	err = Pa_Terminate();
	HANDLE_PA_ERROR(err, "termination");


	return 0;
}


