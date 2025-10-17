#include <iostream>
#include "soundio.h"
#include <fstream>
using namespace std;

void SoundIO::OutputSound(SoundSamples *samples, string filename) {
    ofstream outputfile;
    outputfile.open(filename);

    for(int i=0; i<samples->getLength(); i++){
        outputfile << samples->getsamples()[i] << endl;
    }
}
