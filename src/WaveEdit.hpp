#pragma once

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string>
#include <thread>
#include <vector>


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


////////////////////
// math.cpp
////////////////////

// Integers

inline int eucmodi(int a, int base) {
	int mod = a % base;
	return mod < 0 ? mod + base : mod;
}

inline int mini(int a, int b) {
	return a < b ? a : b;
}

inline int maxi(int a, int b) {
	return a > b ? a : b;
}

/** Limits a value between a minimum and maximum */
inline int clampi(int x, int min, int max) {
	return x > max ? max : x < min ? min : x;
}

// Floats

inline float sgnf(float x) {
	return copysignf(1.0, x);
}

/** Limits a value between a minimum and maximum */
inline float clampf(float x, float min, float max) {
	return x > max ? max : x < min ? min : x;
}

/** If the magnitude of x if less than eps, return 0 */
inline float chopf(float x, float eps) {
	return (-eps < x && x < eps) ? 0.0 : x;
}

inline float rescalef(float x, float xMin, float xMax, float yMin, float yMax) {
	return yMin + (x - xMin) / (xMax - xMin) * (yMax - yMin);
}

inline float crossf(float a, float b, float frac) {
	return (1.0 - frac) * a + frac * b;
}

/** Linearly interpolate an array `p` with index `x`
Assumes that the array at `p` is of length at least ceil(x).
*/
inline float linterpf(const float *p, float x) {
	int xi = x;
	float xf = x - xi;
	if (xf < 1e-6)
		return p[xi];
	else
		return crossf(p[xi], p[xi + 1], xf);
}

/** Returns a random number on [0, 1) */
inline float randf() {
	return (float)rand() / RAND_MAX;
}

inline void complexMult(float *ar, float *ai, float br, float bi) {
	float oldAr = *ar;
	*ar = oldAr * br - *ai * bi;
	*ai = oldAr * bi + *ai * br;
}

void RFFT(const float *in, float *out, int len);
void IRFFT(const float *in, float *out, int len);


////////////////////
// util.cpp
////////////////////

void openBrowser(const char *url);


////////////////////
// bank.cpp
////////////////////

#define BANK_LEN 64
#define BANK_GRID_WIDTH 8
#define BANK_GRID_HEIGHT 8
#define WAVE_LEN 256

enum EffectID {
	PRE_GAIN,
	HARMONIC_SHIFT,
	COMB,
	RING,
	CHEBYSHEV,
	SAMPLE_AND_HOLD,
	QUANTIZATION,
	SLEW,
	LOWPASS,
	HIGHPASS,
	POST_GAIN,
	EFFECTS_LEN
};

extern const char *effectNames[EFFECTS_LEN];

struct Effect {
	float params;
	bool cycle;
	bool normalize;
};

struct Wave {
	float samples[WAVE_LEN];
	/** FFT of wave, interleaved complex numbers */
	float spectrum[WAVE_LEN];
	/** Norm of spectrum */
	float harmonics[WAVE_LEN / 2];
	/** Wave after effects have been applied */
	float postSamples[WAVE_LEN];
	float postSpectrum[WAVE_LEN];
	float postHarmonics[WAVE_LEN / 2];

	float effects[EFFECTS_LEN];
	bool cycle;
	bool normalize;
};

struct Bank {
	Wave waves[BANK_LEN];
};

extern Bank currentBank;

void bankInit();
void updatePost(int waveId);
void commitWave(int waveId);
void commitHarmonics(int waveId);
void clearEffect(int waveId);
void bakeEffect(int waveId);
void randomizeEffect(int waveId);
void bankClear();
void saveBank(const char *filename);
void loadBank(const char *filename);
void saveWaves(const char *dirname);
void setWave(int waveId, const float *wave);
/** Caller must free(). Returns NULL if unsuccessful */
float *loadAudio(const char *filename, int *length);
// TODO
// void bankHistoryPush();
// void bankHistoryPop();
// void bankHistoryRedo();


////////////////////
// waves.cpp
////////////////////

struct WaveFile {
	float samples[WAVE_LEN];
	std::string name;
};

struct WaveDirectory {
	std::vector<WaveFile> waveFiles;
	std::string name;
};

extern std::vector<WaveDirectory> waveDirectories;

void wavesInit();


////////////////////
// audio.cpp
////////////////////

// TODO Some of these should not be exposed in the header
extern float playVolume;
extern float playFrequency;
extern float playFrequencySmooth;
extern bool playEnabled;
extern bool playModeXY;
extern float morphX;
extern float morphY;
extern float morphZ;
extern int playIndex;
extern const char *audioDeviceName;

int resample(const float *in, int inLen, float *out, int outLen, double ratio);
void computeOversample(const float *in, float *out, int len, int oversample);
int audioGetDeviceCount();
const char *audioGetDeviceName(int deviceId);
void audioClose();
void audioOpen(int deviceId);
void audioInit();
void audioDestroy();


////////////////////
// ui.cpp
////////////////////

void uiInit();
void uiRender();
