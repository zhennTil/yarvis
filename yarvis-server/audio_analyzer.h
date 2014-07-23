
#pragma once

#include "BeatDetektor.h"
#include "constants.h"
#include "kiss_fftr.h"
#include <ctime>
#include <memory>
#include <thread>
#include <vector>

class AudioAnalyzer
{
private:
	AudioAnalyzer();

	// Disable copy construction because of the unique_ptr
	AudioAnalyzer(AudioAnalyzer const &);
	AudioAnalyzer &operator=(AudioAnalyzer const &);

	std::unique_ptr<BeatDetektor> beat;
	bool running, error, kill;

	// Analysis data
	kiss_fftr_cfg fft_cfg;
	size_t analysisHead;
	kiss_fft_cpx fft_out[FFT_CHUNK_SIZE / 2 + 1];
	float fft_sym[FFT_CHUNK_SIZE];

	// References to user data struct
	const float *buffer;
	const size_t &bufferHead;
public:
	AudioAnalyzer(const float *buffer, const size_t& bufferHead);
	~AudioAnalyzer();

	void loop();
	void stop();

	void processBuffer(float time);

	bool isRunning();
	bool hasError();
};
