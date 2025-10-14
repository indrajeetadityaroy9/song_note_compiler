#include <cmath>
#include <utility>
#include "SineWave.h"
using namespace std;

/**
 * constructor can set a name of the wave
 * @param wave_Name
 */
SineWave::SineWave(const string& wave_Name) : Wave(wave_Name) { }

/**
 * function will generate a sample of a wave given the time
 * @param time
 * @return
 */
float SineWave::generateFunction(float time) {
    return sinf(2*M_PI*time);
}
