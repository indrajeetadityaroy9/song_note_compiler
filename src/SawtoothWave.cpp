#include <iostream>
#include <math.h>
#include "SawtoothWave.h"
using namespace std;

SawtoothWave::SawtoothWave(const string &wave_Name) : Wave(wave_Name){}

float SawtoothWave::generateFunction(float time) {
    return time - floor(time + 0.5);
}
