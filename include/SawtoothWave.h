#ifndef SONG_NOTE_COMPILER_SAWTOOTHWAVE_H
#define SONG_NOTE_COMPILER_SAWTOOTHWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class SawtoothWave : public Wave{

public:
    explicit SawtoothWave(const string& waveName);
    float generateFunction(float time) override;
};


#endif
