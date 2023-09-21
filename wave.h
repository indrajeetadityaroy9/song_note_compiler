#ifndef PROJECT02_WAVE_H
#define PROJECT02_WAVE_H
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

#endif //PROJECT02_WAVE_H
