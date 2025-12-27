#include "wave.h"
#include "SquareWave.h"
using namespace std;

SquareWave::SquareWave(const string &waveName) : Wave(waveName){}

float SquareWave::generateFunction(float time) {
    // Use phase comparison instead of computing full sine
    float phase = normalizePhase(time);
    return phase < 0.5f ? 1.0f : -1.0f;
}
