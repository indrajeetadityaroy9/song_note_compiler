#ifndef SONG_NOTE_COMPILER_NOTE_H
#define SONG_NOTE_COMPILER_NOTE_H

#include <string>
#include <vector>

struct Note {
    std::vector<std::string> noteNames;  // Note names (e.g., "C4", "A#3") or empty for rest
    std::string durationStr;              // Duration notation (e.g., "q", "h.", "et")
    float duration = 0.0f;                // Duration in seconds
    float volume = 1.0f;                  // Volume (0.0 - 1.0)
    int trackId = 1;                      // Track this note belongs to

    // MusicXML-specific fields (used for import/export fidelity)
    int measureNumber = 0;                // Which measure this note belongs to
    int divisions = 0;                    // MusicXML divisions value (0 = use default)
    int mxmlDuration = 0;                 // MusicXML duration value (0 = calculate from durationStr)
};

#endif
