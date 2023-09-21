#include <iostream>
#include <cmath>
#include "wave.h"
#include "SquareWave.h"
using namespace std;

/**
 *
 * @param waveName
 */
SquareWave::SquareWave(const string &waveName) : Wave(waveName){}

/**
 *
 * @param time
 * @return
 */
float SquareWave::generateFunction(float time) {
    return sinf(2*M_PI*time) >= 0.0 ? 1.0:-1.0;
}
