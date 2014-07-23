
#include "audio_analyzer.h"

AudioAnalyzer::AudioAnalyzer(const float *buffer, const size_t& bufferHead)
	: running(false),
	error(false),
	kill(false),
	buffer(buffer),
	bufferHead(bufferHead),
	analysisHead(0),
	fftHalfOut(NULL)
{
	// Instantiate beat detektor
	this->beat = std::unique_ptr<BeatDetektor>(new BeatDetektor(100.f, 199.f));

	// Allocate FFT memory
	this->fftCfg = kiss_fftr_alloc(FFT_CHUNK_SIZE, 0, NULL, NULL);
}

AudioAnalyzer::AudioAnalyzer(const float *buffer, const size_t& bufferHead, float *fftHalfOut)
	: running(false),
	error(false),
	kill(false),
	buffer(buffer),
	bufferHead(bufferHead),
	analysisHead(0),
	fftHalfOut(fftHalfOut)
{
	// Instantiate beat detektor
	this->beat = std::unique_ptr<BeatDetektor>(new BeatDetektor(100.f, 199.f));

	// Allocate FFT memory
	this->fftCfg = kiss_fftr_alloc(FFT_CHUNK_SIZE, 0, NULL, NULL);
}

void AudioAnalyzer::loop()
{
	float startTime = (float)clock() / CLOCKS_PER_SEC;

	while (!kill)
	{
		std::this_thread::yield();

		while (bufferHead > analysisHead + FFT_CHUNK_SIZE
			|| bufferHead < analysisHead)
		{
			processBuffer((float)clock() / CLOCKS_PER_SEC - startTime);
		}
	}
}

float AudioAnalyzer::hann(size_t n)
{
	return .5f *
	(
		1 - cosf(
			(PI_DOUBLE * n) / (FFT_CHUNK_SIZE - 1)
		)
	);
}

void AudioAnalyzer::processBuffer(float time)
{
	// Apply Hann window
	float fftIn[FFT_CHUNK_SIZE];
	for (size_t i = 0; i < FFT_CHUNK_SIZE; ++i)
	{
		fftIn[i] = buffer[analysisHead + i] * hann(i);
	}

	// Compute FFT
	kiss_fftr(fftCfg, fftIn, fftOut);

	// Convert to symmetric real format for BeatDetektor
	kiss_fft_cpx out;
	for (size_t i = 0; i < FFT_CHUNK_SIZE/4; ++i)
	{
		out = fftOut[i];
		fftSym[i] = sqrtf(out.r*out.r + out.i*out.i);
		fftSym[FFT_CHUNK_SIZE/2 - 1 - i] = fftSym[i];
		
		//fftSym[i] = sqrtf(out.r*out.r + out.i*out.i);
	}
	out = fftOut[FFT_CHUNK_SIZE / 4];
	fftSym[FFT_CHUNK_SIZE / 4] = sqrtf(out.r*out.r + out.i*out.i);

	// Write to auxiliary output if specified
	if (fftHalfOut != NULL)
	{
		for (size_t i = 0; i < FFT_CHUNK_SIZE / 4; ++i)
		{
			fftHalfOut[i] = fftSym[i];
		}
	}

	// Process with BeatDetektor
	beat->process(time, std::vector<float>(fftSym, fftSym + FFT_CHUNK_SIZE));
	
	if (analysisHead == AUDIO_BUFFER_SIZE - FFT_CHUNK_SIZE)
		analysisHead = 0;
	else
		analysisHead = analysisHead + FFT_CHUNK_SIZE;
}

void AudioAnalyzer::stop()
{
	kill = true;
}

AudioAnalyzer::~AudioAnalyzer()
{

	// Free the FFT memory
	kiss_fftr_free(fftCfg);
}
