#include <iostream>
#include "soundio.h"
#include <fstream>
using namespace std;

/**
 * function to write to the file
 * @param samples
 * @param filename
 */
void SoundIO::OutputSound(SoundSamples *samples, string filename) {
    ofstream outputfile;
    outputfile.open(filename);

    for(int i=0; i<samples->getLength(); i++){
        outputfile << samples->getsamples()[i] << endl;
    }
}
