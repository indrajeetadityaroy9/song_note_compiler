#ifndef SONG_NOTE_COMPILER_SOUNDIO_H
#define SONG_NOTE_COMPILER_SOUNDIO_H
#include "SoundSamples.h"
#include <string>
using namespace std;

class SoundIO{
public:
    /**
     * function to write to the file
     * @param samples
     * @param filename
     */
    static void OutputSound(SoundSamples *samples, string filename );
};
#endif //SONG_NOTE_COMPILER_SOUNDIO_H
