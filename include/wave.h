#ifndef SONG_NOTE_COMPILER_WAVE_H
#define SONG_NOTE_COMPILER_WAVE_H
#include <iostream>
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
};

#endif
