#include "SawtoothWave.h"
using namespace std;

SawtoothWave::SawtoothWave(const string &wave_Name) : Wave(wave_Name){}

float SawtoothWave::generateFunction(float time) {
    // Sawtooth wave: ramps from -1 to 1, output range -1 to 1
    float phase = normalizePhase(time);
    return 2.0f * phase - 1.0f;
}
