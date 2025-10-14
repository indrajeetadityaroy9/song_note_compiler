#ifndef SONG_NOTE_COMPILER_SINEWAVE_H
#define SONG_NOTE_COMPILER_SINEWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class SineWave : public Wave{

public:
    /**
     * constructor can set a name of the wave
     * @param waveName
     */
    explicit SineWave(const string& waveName);
    /**
     * function will generate a sample of a wave given the time
     * @param time
     * @return
     */
    float generateFunction(float time) override;
};

#endif //SONG_NOTE_COMPILER_SINEWAVE_H
