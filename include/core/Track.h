#ifndef SONG_NOTE_COMPILER_TRACK_H
#define SONG_NOTE_COMPILER_TRACK_H

#include <string>

struct Track {
    int id = 1;
    int waveType = 1;       // 1=Sine, 2=Square, 3=Triangle, 4=Sawtooth
    float volume = 1.0f;    // Track volume (0.0 - 1.0)
    std::string name;       // Display name (e.g., "Track 1")
    std::string partId;     // MusicXML part ID (e.g., "P1", "P2")
};

#endif
