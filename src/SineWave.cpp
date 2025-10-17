#include <cmath>
#include <utility>
#include "SineWave.h"
using namespace std;

SineWave::SineWave(const string& wave_Name) : Wave(wave_Name) { }

float SineWave::generateFunction(float time) {
    return sinf(2*M_PI*time);
}
