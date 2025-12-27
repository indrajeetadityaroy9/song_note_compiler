#include <cmath>
#include "TriangleWave.h"
using namespace std;

TriangleWave::TriangleWave(const string &wave_Name) : Wave(wave_Name){}

float TriangleWave::generateFunction(float time) {
    // Triangle wave: ramps up then down, output range -1 to 1
    float phase = normalizePhase(time);
    return 4.0f * std::fabs(phase - 0.5f) - 1.0f;
}
