#ifndef SONG_NOTE_COMPILER_SONGGENERATOR_H
#define SONG_NOTE_COMPILER_SONGGENERATOR_H

#include <vector>
#include <map>
#include "Note.h"
#include "Track.h"
#include "SoundSamples.h"
#include "wave.h"

class SongGenerator {
public:
    // Generate audio for a single track
    static SoundSamples* generateTrackAudio(
        const std::vector<Note>& trackMelody,
        const Track& trackInfo,
        float sampleRate
    );

    // Mix multiple tracks into a single audio buffer
    static SoundSamples* mixTracks(
        const std::map<int, SoundSamples*>& trackAudio,
        float sampleRate
    );

    // Create the appropriate Wave object for a given type
    static Wave* createWave(int waveType);

private:
    // Generate a note directly into a buffer at a given position
    static void generateNoteIntoBuffer(
        float* buffer, int writePos, Wave* wave,
        float frequency, float sampleRate, float duration, float volume
    );
};

#endif
