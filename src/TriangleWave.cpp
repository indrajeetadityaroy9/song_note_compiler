#include <cmath>
#include <utility>
#include "TriangleWave.h"
using namespace std;

TriangleWave::TriangleWave(const string &wave_Name) : Wave(wave_Name){}

float TriangleWave::generateFunction(float time) {
    return abs(time - floor(time + 0.5));
}
