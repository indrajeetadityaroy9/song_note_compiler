#include <cmath>
#include <string>
#include <utility>
#include "wave.h"
using namespace std;

Wave::Wave(string  name) : wave_name(std::move(name)) { }

SoundSamples * Wave::generateSamples(float frequency, float samplerate, float duration) {

    int length = static_cast<int>(std::round(samplerate * duration));

    float *samples;
    samples = (float *) malloc(length * sizeof(float));

    for (int i = 0; i < length; i++) {
        samples[i] = generateFunction(i*frequency/samplerate);
    }

    auto *S = new SoundSamples(samples,length,samplerate);
    return S;
}

float Wave::generateFunction(float time) {
    return 0;
}
