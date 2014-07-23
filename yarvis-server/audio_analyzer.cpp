
#include "audio_analyzer.h"
#include <iostream>

AudioAnalyzer::AudioAnalyzer(const float *buffer, const size_t& bufferHead)
	: running(false),
	error(false),
	kill(false),
	buffer(buffer),
	bufferHead(bufferHead),
	analysisHead(0)
{
	// Instantiate beat detektor
	this->beat = std::unique_ptr<BeatDetektor>(new BeatDetektor(100.f, 199.f));

	// Allocate FFT memory
	this->fft_cfg = kiss_fftr_alloc(FFT_CHUNK_SIZE, 0, NULL, NULL);
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

void AudioAnalyzer::processBuffer(float time)
{
	// TODO: Apply Hann window

	// Compute FFT
	kiss_fftr(fft_cfg, buffer + analysisHead, fft_out);

	// Convert to symmetric real format for BeatDetektor
	for (size_t i = 0; i < FFT_CHUNK_SIZE/2; ++i)
	{
		fft_sym[i] = fft_out[i].r;
		fft_sym[FFT_CHUNK_SIZE - 1 - i] = fft_out[i].r;
	}
	fft_sym[FFT_CHUNK_SIZE / 2] = fft_out[FFT_CHUNK_SIZE / 2].r;

	// Process with BeatDetektor
	beat->process(time, std::vector<float>(fft_sym, fft_sym + FFT_CHUNK_SIZE));
	
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
	kiss_fftr_free(fft_cfg);
}
