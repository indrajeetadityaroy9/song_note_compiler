#include <iostream>
#include <math.h>
#include "SawtoothWave.h"
using namespace std;

/**
 * constructor can set a name of the wave
 * @param wave_Name
 */
SawtoothWave::SawtoothWave(const string &wave_Name) : Wave(wave_Name){}

/**
 * function will generate a sample of a wave given the time
 * @param time
 * @return
 */
float SawtoothWave::generateFunction(float time) {
    return time - floor(time + 0.5);
}
