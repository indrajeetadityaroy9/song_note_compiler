#include <cmath>
#include <string>
#include <utility>
#include "wave.h"
using namespace std;

/**
 * constructor can set a name of the wave
 * @param name
 */
Wave::Wave(string  name) : wave_name(std::move(name)) { }

/**
 * function creates the samples for the specified wave at the specified sample rate for the specified duration
 * @param frequency
 * @param samplerate
 * @param duration
 * @return
 */
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

/**
 * function generates a sample of awave given the time
 * @param time
 * @return
 */
float Wave::generateFunction(float time) {
    return 0;
}
