#ifndef SONG_NOTE_COMPILER_SQUAREWAVE_H
#define SONG_NOTE_COMPILER_SQUAREWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class SquareWave : public Wave{

public:
    explicit SquareWave(const string& waveName);
    float generateFunction(float time) override;
};
#endif
