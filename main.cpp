#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "soundio.h"
#include <cstring>

int main(int argc, char *argv[]) {

    //wave
    Wave *w;
    //sound Samples
    SoundSamples *s;
    //sample rate
    float sample_rate;
    //frequency
    float frequency;
    //duration
    float duration;
    //output filename
    string filename;

    if (argc < 6) {
        cout << "INPUT 5 ARGUMENTS IN THE ORDER 1->5" << endl;
        cout << "1.Wave Type 2.Sample rate 3.frequency 4.duration 5.File name" << endl;
    } else if (argc > 6) {
        cout << "Too many arguments" << endl;
    } else if (argc == 6) {
        if (strchr(argv[1], '1')) {
            w = new SineWave("SineWave");
        }
        if (strchr(argv[1], '2')) {
            w = new SquareWave("SquareWave");
        }
        if (strchr(argv[1], '3')) {
            w = new TriangleWave("TriangleWave");
        }
        if (strchr(argv[1], '4')) {
            w = new SawtoothWave("SawtoothWave");
        }

        sample_rate = strtof(argv[2], nullptr);
        frequency = strtof(argv[3], nullptr);
        duration = strtof(argv[4], nullptr);
        filename = argv[5];

        s = w->generateSamples(frequency, sample_rate, duration);
        SoundIO::OutputSound(s, filename);
        cout << "WAVE GENERATED" << endl;
    }
}
