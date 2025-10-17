#ifndef SONG_NOTE_COMPILER_SOUNDIO_H
#define SONG_NOTE_COMPILER_SOUNDIO_H
#include "SoundSamples.h"
#include <string>
using namespace std;

class SoundIO{
public:
    static void OutputSound(SoundSamples *samples, string filename );
};
#endif
