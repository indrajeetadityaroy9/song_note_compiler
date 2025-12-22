#ifndef SONG_NOTE_COMPILER_FILEPARSER_H
#define SONG_NOTE_COMPILER_FILEPARSER_H

#include <string>
#include <vector>
#include "Note.h"
#include "SongContext.h"

class FileParser {
public:
    // Read notes from a file, populating the context with tempo/tracks
    static std::vector<Note> readNotesFromFile(
        const std::string& filename,
        SongContext& ctx
    );

    // Save notes to a file
    static bool saveNotesToFile(
        const std::vector<Note>& notes,
        const SongContext& ctx,
        const std::string& filename
    );

    // Check if a string represents a chord (contains comma)
    static bool isChord(const std::string& noteStr);

    // Parse a chord string into individual note names
    static std::vector<std::string> parseChord(const std::string& chordStr);
};

#endif
