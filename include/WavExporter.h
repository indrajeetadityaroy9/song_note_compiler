#ifndef SONG_NOTE_COMPILER_WAVEXPORTER_H
#define SONG_NOTE_COMPILER_WAVEXPORTER_H

#include "SoundSamples.h"
#include <string>

class WavExporter {
public:
    /**
     * Export SoundSamples to WAV file format
     * @param samples - Pointer to SoundSamples object containing audio data
     * @param filename - Output filename (e.g., "output.wav")
     * @param bitDepth - Bit depth for WAV file (8, 16, 24, or 32), default is 16
     * @return true if export successful, false otherwise
     */
    static bool exportToWav(const SoundSamples* samples,
                           const std::string& filename,
                           int bitDepth = 16);
};

#endif //SONG_NOTE_COMPILER_WAVEXPORTER_H
