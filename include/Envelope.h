#ifndef ENVELOPE_H
#define ENVELOPE_H

struct EnvelopeParams {
    float attack = 0.01f;   // Attack time in seconds
    float decay = 0.05f;    // Decay time in seconds
    float sustain = 0.8f;   // Sustain level (0.0 - 1.0)
    float release = 0.08f;  // Release time in seconds
};

class Envelope {
public:
    // Apply ADSR envelope to a sample buffer in-place
    static void apply(float* samples, int length, float sampleRate,
                      const EnvelopeParams& params);

    // Get envelope multiplier at a given time position
    static float getMultiplier(float time, float duration,
                               const EnvelopeParams& params);
};

#endif
