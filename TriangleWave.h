#ifndef PROJECT02_TRIANGLEWAVE_H
#define PROJECT02_TRIANGLEWAVE_H
#include <iostream>
#include <string>
#include "wave.h"
using namespace std;

class TriangleWave : public Wave{

public:
    /**
     * constructor can set a name of the wave
     * @param waveName
     */
    explicit TriangleWave(const string& waveName);
    /**
     * function will generate a sample of a wave given the time
     * @param time
     * @return
     */
    float generateFunction(float time) override;
};

#endif //PROJECT02_TRIANGLEWAVE_H
