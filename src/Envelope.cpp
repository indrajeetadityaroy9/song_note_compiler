#include "Envelope.h"
#include <algorithm>
#include <cmath>

float Envelope::getMultiplier(float time, float duration,
                              const EnvelopeParams& params) {
    // Clamp parameters to reasonable values
    float attack = std::max(0.001f, params.attack);
    float decay = std::max(0.001f, params.decay);
    float sustain = std::clamp(params.sustain, 0.0f, 1.0f);
    float release = std::max(0.001f, params.release);

    // Calculate release start time
    float releaseStart = std::max(0.0f, duration - release);

    if (time < 0.0f) {
        return 0.0f;
    }

    // Attack phase
    if (time < attack) {
        return time / attack;
    }

    // Decay phase
    if (time < attack + decay) {
        float decayProgress = (time - attack) / decay;
        return 1.0f - (1.0f - sustain) * decayProgress;
    }

    // Release phase
    if (time >= releaseStart) {
        float releaseProgress = (time - releaseStart) / release;
        releaseProgress = std::clamp(releaseProgress, 0.0f, 1.0f);
        return sustain * (1.0f - releaseProgress);
    }

    // Sustain phase
    return sustain;
}

void Envelope::apply(float* samples, int length, float sampleRate,
                     const EnvelopeParams& params) {
    if (!samples || length <= 0 || sampleRate <= 0) {
        return;
    }

    float duration = static_cast<float>(length) / sampleRate;

    for (int i = 0; i < length; i++) {
        float time = static_cast<float>(i) / sampleRate;
        float multiplier = getMultiplier(time, duration, params);
        samples[i] *= multiplier;
    }
}
