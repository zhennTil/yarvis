
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include "BeatDetektor.h"
#include "portaudio.h"

using namespace std;

#define HANDLE_PA_ERROR(err, situation) if(err!=paNoError){ cerr << "ERROR in PortAudio " << situation << ": " << Pa_GetErrorText(err) << endl; return 1; }

typedef struct
{
	
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

	unsigned long i;
	for ( i = 0; i<framesPerBuffer; i++ )
	{
		// TODO: FFT
	}

	return 0;
}

PaDeviceIndex selectInputDevice()
{
	// TODO: Provide fallback device selector if Pa_GetDefaultInputDevice() fails. Even better, with a cmake option.

	/*PaDeviceIndex numDevices = Pa_GetDeviceCount();
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
	cin >> selectedI;*/
	
	//cout << Pa_GetHostApiInfo(deviceInfo->hostApi)->name << endl;
	
	return Pa_GetDefaultInputDevice();
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
	// Initialize user data
	PaUserData data;

	// Open audio input
	PaError err = Pa_Initialize();
	HANDLE_PA_ERROR(err, "initialization");

	PaDeviceIndex inputIndex = selectInputDevice();

	PaStream *stream;
	err = openInputStream(&stream, inputIndex, &data);
	HANDLE_PA_ERROR(err, "stream open");
	
	err = Pa_StartStream(stream);
	HANDLE_PA_ERROR(err, "stream start");

	// Instantiate beat detektor
	BeatDetektor bd(100.f, 199.f);

	// Check command input
	char cmd = ' ';
	cout << "Entering loop" << endl;
	do
	{
		//this_thread::yield();
		cin >> cmd;
	} while (cmd != 'q');
	cout << "Exiting loop" << endl;

	// Terminate audio input
	err = Pa_StopStream(stream);
	HANDLE_PA_ERROR(err, "stream stop");

	err = Pa_Terminate();
	HANDLE_PA_ERROR(err, "termination");

	return 0;
}


