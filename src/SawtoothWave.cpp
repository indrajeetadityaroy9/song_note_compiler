#include <iostream>
#include <cmath>
#include "SawtoothWave.h"
using namespace std;

SawtoothWave::SawtoothWave(const string &wave_Name) : Wave(wave_Name){}

float SawtoothWave::generateFunction(float time) {
    // Sawtooth wave: ramps from -1 to 1, output range -1 to 1
    float phase = fmodf(time, 1.0f);
    if (phase < 0) phase += 1.0f;
    return 2.0f * phase - 1.0f;
}
