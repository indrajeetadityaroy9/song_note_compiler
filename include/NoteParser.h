#ifndef SONG_NOTE_COMPILER_NOTEPARSER_H
#define SONG_NOTE_COMPILER_NOTEPARSER_H

#include <string>
#include <map>

class NoteParser {
public:
    static float parseNote(const std::string& noteStr);
    static float getNoteFrequency(const std::string& noteName, int octave);
    static bool isValidNote(const std::string& noteStr);

private:
    static std::string normalizeNoteName(const std::string& noteName);
    static float midiToFrequency(int midiNote);
    static const std::map<std::string, int> noteToSemitone;
};

#endif
