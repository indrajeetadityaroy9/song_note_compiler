#ifndef SONG_NOTE_COMPILER_WAVEXPORTER_H
#define SONG_NOTE_COMPILER_WAVEXPORTER_H

#include "SoundSamples.h"
#include <string>

class WavExporter {
public:
    static bool exportToWav(const SoundSamples* samples,
                           const std::string& filename,
                           int bitDepth = 16);
};

#endif
