#ifndef SONG_NOTE_COMPILER_RHYTHMPARSER_H
#define SONG_NOTE_COMPILER_RHYTHMPARSER_H

#include <string>
#include <vector>

// Forward declaration
struct MusicXMLDuration;

class RhythmParser {
public:
    // Existing methods
    static float parseRhythm(const std::string& rhythmStr, float tempo = 120.0f, const std::string& timeSignature = "4/4");
    static float beatsToSeconds(float beats, float tempo);
    static float parseTimeSignature(const std::string& timeSignature);
    static float getNoteBeatValue(char noteType);

    // MusicXML conversion methods
    static MusicXMLDuration durationStrToMusicXML(const std::string& durationStr, int divisions);
    static std::string musicXMLToDurationStr(const MusicXMLDuration& mxmlDur);

    // Divisions calculation
    static int calculateDivisions(const std::vector<std::string>& durationStrs);

    // Convert beats to MusicXML duration units
    static int beatsToMusicXMLDuration(float beats, int divisions);
    static float musicXMLDurationToBeats(int duration, int divisions);

    // Type string conversion
    static std::string beatsToTypeString(float beats);
    static float typeStringToBeats(const std::string& type);

    // Default divisions per quarter note (24 handles triplets and dotted notes cleanly)
    static const int DEFAULT_DIVISIONS = 24;
};

#endif