#include <cmath>
#include <utility>
#include "TriangleWave.h"
using namespace std;

/**
 *
 * @param wave_Name
 */
TriangleWave::TriangleWave(const string &wave_Name) : Wave(wave_Name){}

/**
 *
 * @param time
 * @return
 */
float TriangleWave::generateFunction(float time) {
    return abs(time - floor(time + 0.5));
}
