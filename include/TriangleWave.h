#ifndef SONG_NOTE_COMPILER_TRIANGLEWAVE_H
#define SONG_NOTE_COMPILER_TRIANGLEWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class TriangleWave : public Wave{

public:
    explicit TriangleWave(const string& waveName);
    float generateFunction(float time) override;
};

#endif
