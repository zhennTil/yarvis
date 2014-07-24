
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

	bool running, error, kill;

	// Analysis data
	kiss_fftr_cfg fftCfg;
	size_t analysisHead;
	kiss_fft_cpx fftOut[FFT_CHUNK_SIZE / 2 + 1];
	float fftAmp[FFT_CHUNK_SIZE / 2];
	float *fftHalfOut;

	// References to user data struct
	const float *buffer;
	const size_t &bufferHead;

	float hann(size_t n);
public:
	AudioAnalyzer(const float *buffer, const size_t& bufferHead);
	AudioAnalyzer(const float *buffer, const size_t& bufferHead, float *fftHalfOut);
	~AudioAnalyzer();

	std::unique_ptr<BeatDetektor> beat;

	void loop();
	void stop();

	void processBuffer(float time);

	bool isRunning();
	bool hasError();
};
