#ifndef SONG_NOTE_COMPILER_WAVE_H
#define SONG_NOTE_COMPILER_WAVE_H
#include <cmath>
#include <string>
#include "SoundSamples.h"
using namespace std;

class Wave{
private:
    string wave_name;
public:
    explicit Wave(string  name);
    virtual ~Wave() = default;
    SoundSamples *generateSamples(float frequency, float samplerate, float duration);
    virtual float generateFunction(float time);

    // Shared utility for phase normalization (0.0 to 1.0 range)
    static float normalizePhase(float time) {
        float phase = std::fmod(time, 1.0f);
        return phase < 0 ? phase + 1.0f : phase;
    }
};

#endif
