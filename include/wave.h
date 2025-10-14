#ifndef SONG_NOTE_COMPILER_WAVE_H
#define SONG_NOTE_COMPILER_WAVE_H
#include <iostream>
#include <string>
#include "SoundSamples.h"
using namespace std;

class Wave{
private:
    /**
     * wane name
     */
    string wave_name;
public:
    /**
     * constructor that takes in the name of the wave
     * @param name
     */
    explicit Wave(string  name);

    /**
     * virtual destructor for proper cleanup of derived classes
     */
    virtual ~Wave() = default;

    /**
     * creates the samples for the specified wave at the specified sample rate for the specified duration
     * @param frequency
     * @param samplerate
     * @param duration
     * @return
     */
    SoundSamples *generateSamples(float frequency, float samplerate, float duration);
    /**
     * function generate a sample of a wave given the time
     * @param time
     * @return
     */
    virtual float generateFunction(float time);
};

#endif //SONG_NOTE_COMPILER_WAVE_H
