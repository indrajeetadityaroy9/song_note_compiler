#ifndef SONG_NOTE_COMPILER_NOTE_H
#define SONG_NOTE_COMPILER_NOTE_H

#include <string>
#include <vector>

struct Note {
    std::vector<std::string> noteNames;
    std::string durationStr;
    float duration;
    float volume;
    int trackId;
};

#endif
