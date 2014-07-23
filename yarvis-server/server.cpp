
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include "BeatDetektor.h"
#include "portaudio.h"
#include "kiss_fftr.h"

using namespace std;

#define HANDLE_PA_ERROR(err, situation) if(err!=paNoError){ cerr << "ERROR in PortAudio " << situation << ": " << Pa_GetErrorText(err) << endl; return 1; }

typedef struct
{
	vector<float> buffer;
	size_t buffer_size;
	bool lock;
	bool updated;
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

	// Replace buffer data
	ud->lock = true;
	if (!ud->updated)
	{
		ud->buffer_size = 0;
	}

	ud->buffer.resize(ud->buffer_size + framesPerBuffer);
	for (unsigned long i = 0; i < framesPerBuffer; i++)
	{
		ud->buffer[ud->buffer_size + i] = in[i];
	}
	ud->buffer_size += framesPerBuffer;

	ud->updated = true;
	ud->lock = false;

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

void processBuffer(const vector<float>& buffer, size_t buffer_size, kiss_fftr_cfg& fft_cfg, BeatDetektor*& beat, float time)
{
	kiss_fft_cpx fft_out[65];
	float fft_sym[128];

	// Ensure buffer is multiple of 128
	buffer_size -= buffer_size % 128;

	// TODO: Apply Hann window

	// Compute FFT
	for (size_t bufferI = 0; bufferI < buffer_size; bufferI += 128)
	{
		kiss_fftr(fft_cfg, buffer.data() + bufferI, fft_out);

		// Convert to symmetric real format for BeatDetektor
		for (size_t i = 0; i < 64; ++i)
		{
			fft_sym[i] = fft_out[i].r;
			fft_sym[127 - i] = fft_out[i].r;
		}
		fft_sym[64] = fft_out[64].r;

		// Process with BeatDetektor
		beat->process(time, vector<float>(fft_sym, fft_sym+128));
	}
}

int main(int argc, char* argv[])
{
	// Initialize shared resources
	kiss_fftr_cfg fft_cfg = kiss_fftr_alloc(128, 0, NULL, NULL);
	PaUserData data;
	data.updated = false;
	data.lock = false;
	vector<float> backBuffer;

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
	BeatDetektor *bd = new BeatDetektor(100.f, 199.f);
	double startTime = Pa_GetStreamTime(stream);

	// Check command input
	char cmd = ' ';
	cout << "Entering loop" << endl;
	do
	{
		//cin >> cmd;

		// TODO: Place the buffer processing code in a separate thread, so we can allow the command input to block.
		this_thread::yield();

		// In case an interrupt is running, wait for it to finish.
		while (data.lock) this_thread::yield();

		// Has the interrupt updated its buffer?
		if (data.updated && data.buffer_size >= 128)
		{
			// Swap the buffers and clear the updated-flag.
			data.buffer.swap(backBuffer);
			data.updated = false;

			// Finally, process the swapped-out buffer.
			processBuffer(backBuffer, data.buffer_size, fft_cfg, bd, (float)(Pa_GetStreamTime(stream) - startTime));
		}
	} while (cmd != 'q');
	cout << "Exiting loop" << endl;

	// Free BeatDetektor
	delete bd;

	// Terminate audio input
	err = Pa_StopStream(stream);
	HANDLE_PA_ERROR(err, "stream stop");

	err = Pa_Terminate();
	HANDLE_PA_ERROR(err, "termination");

	// Free the FFT memory
	kiss_fftr_free(fft_cfg);

	return 0;
}


