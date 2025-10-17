#ifndef SONG_NOTE_COMPILER_RHYTHMPARSER_H
#define SONG_NOTE_COMPILER_RHYTHMPARSER_H

#include <string>

class RhythmParser {
public:
    static float parseRhythm(const std::string& rhythmStr, float tempo = 120.0f, const std::string& timeSignature = "4/4");
    static float beatsToSeconds(float beats, float tempo);
    static float parseTimeSignature(const std::string& timeSignature);

private:
    static float getNoteBeatValue(char noteType);
};

#endif