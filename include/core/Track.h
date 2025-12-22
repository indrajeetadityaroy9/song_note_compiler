#ifndef SONG_NOTE_COMPILER_TRACK_H
#define SONG_NOTE_COMPILER_TRACK_H

#include <string>

struct Track {
    int id;
    int waveType;  // 1=Sine, 2=Square, 3=Triangle, 4=Sawtooth
    float volume;
    std::string name;
};

#endif
