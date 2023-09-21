#ifndef PROJECT02_SQUAREWAVE_H
#define PROJECT02_SQUAREWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class SquareWave : public Wave{

public:
    /**
     * constructor can set a name of the wave
     * @param waveName
     */
    explicit SquareWave(const string& waveName);
    /**
     * function will generate a sample of a wave given the time
     * @param time
     * @return
     */
    float generateFunction(float time) override;
};
#endif //PROJECT02_SQUAREWAVE_H
