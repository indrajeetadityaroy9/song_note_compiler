#include <cmath>
#include <string>
#include <utility>
#include "wave.h"
using namespace std;

Wave::Wave(string  name) : wave_name(std::move(name)) { }

SoundSamples * Wave::generateSamples(float frequency, float samplerate, float duration) {
    int length = static_cast<int>(std::round(samplerate * duration));

    // Allocate directly into SoundSamples to avoid double copy
    auto *S = new SoundSamples(length, samplerate);
    float *samples = S->getsamples();

    // Use phase accumulation instead of computing i*frequency/samplerate each iteration
    float phaseIncrement = frequency / samplerate;
    float phase = 0.0f;

    for (int i = 0; i < length; i++) {
        samples[i] = generateFunction(phase);
        phase += phaseIncrement;
    }

    return S;
}

float Wave::generateFunction(float time) {
    return 0;
}
