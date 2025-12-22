#include <cmath>
#include <utility>
#include "TriangleWave.h"
using namespace std;

TriangleWave::TriangleWave(const string &wave_Name) : Wave(wave_Name){}

float TriangleWave::generateFunction(float time) {
    // Triangle wave: ramps up then down, output range -1 to 1
    float phase = fmodf(time, 1.0f);
    if (phase < 0) phase += 1.0f;
    return 4.0f * fabsf(phase - 0.5f) - 1.0f;
}
