
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
	ud->buffer.resize(framesPerBuffer);
	ud->buffer_size = framesPerBuffer;
	unsigned long i;
	for ( i = 0; i<framesPerBuffer; i++ )
	{
		ud->buffer[i] = in[i];
	}
	ud->lock = false;
	ud->updated = true;

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

void processBuffer(const vector<float>& buffer, size_t buffer_size, kiss_fftr_cfg& fft_cfg, BeatDetektor& beat, float time)
{
	static size_t fft_size = 0;
	static vector<kiss_fft_cpx> fft_out;
	static vector<float> fft_sym;

	// Ensure buffer is of even size
	if (buffer_size % 2 > 0)
	{
		--buffer_size;
	}

	// TODO: Apply Hann window


	// Allocate FFT memory if needed
	if (fft_size != buffer_size)
	{
		if (fft_cfg != NULL) kiss_fftr_free(fft_cfg);
		fft_cfg = kiss_fftr_alloc((int)buffer_size, 0, NULL, NULL);
		fft_size = buffer_size;
		fft_out.resize(fft_size / 2);
		fft_sym.resize(fft_size);
	}
	
	// Compute FFT
	kiss_fftr(fft_cfg, buffer.data(), fft_out.data());

	// Convert to symmetric real format for BeatDetektor
	for (size_t i = 0; i < fft_size / 2; ++i)
	{
		fft_sym[i] = fft_out[i].r;
		fft_sym[fft_size - 1 - i] = fft_out[i].r;
	}

	// Process with BeatDetektor
	beat.process(time, fft_sym);
}

int main(int argc, char* argv[])
{
	// Initialize shared resources
	kiss_fftr_cfg fft_cfg = NULL;
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
	BeatDetektor bd(100.f, 199.f);
	double startTime = Pa_GetStreamTime(stream);

	// Check command input
	char cmd = ' ';
	cout << "Entering loop" << endl;
	do
	{
		//cin >> cmd;

		// TODO: Place the buffer processing code in a separate thread, so we can allow the command input to block.
		this_thread::yield();

		// Has the interrupt updated its buffer?
		if (data.updated)
		{
			// In case another interrupt has already started, wait for it to finish.
			while (data.lock) this_thread::yield();
			// Swap the buffers and clear the updated-flag.
			data.buffer.swap(backBuffer);
			data.updated = false;

			// Finally, process the swapped-out buffer.
			processBuffer(backBuffer, data.buffer_size, fft_cfg, bd, (float)(Pa_GetStreamTime(stream) - startTime));
		}
	} while (cmd != 'q');
	cout << "Exiting loop" << endl;

	// Terminate audio input
	err = Pa_StopStream(stream);
	HANDLE_PA_ERROR(err, "stream stop");

	err = Pa_Terminate();
	HANDLE_PA_ERROR(err, "termination");

	// Free the FFT memory, if allocated
	if (fft_cfg != NULL)
		kiss_fftr_free(fft_cfg);

	return 0;
}


