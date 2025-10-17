#ifndef SONG_NOTE_COMPILER_SINEWAVE_H
#define SONG_NOTE_COMPILER_SINEWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class SineWave : public Wave{

public:
    explicit SineWave(const string& waveName);
    float generateFunction(float time) override;
};

#endif
