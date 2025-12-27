#ifndef SONG_NOTE_COMPILER_NOTEPARSER_H
#define SONG_NOTE_COMPILER_NOTEPARSER_H

#include <string>
#include <map>

// Forward declaration
struct MusicXMLPitch;

class NoteParser {
public:
    // Existing methods - note string to frequency
    static float parseNote(const std::string& noteStr);
    static float getNoteFrequency(const std::string& noteName, int octave);
    static bool isValidNote(const std::string& noteStr);

    // Reverse operations for MusicXML export
    static MusicXMLPitch noteNameToPitch(const std::string& noteName);
    static MusicXMLPitch frequencyToPitch(float frequency);
    static std::string pitchToNoteName(const MusicXMLPitch& pitch);

    // MIDI conversion utilities
    static int noteNameToMidi(const std::string& noteName);
    static std::string midiToNoteName(int midiNote);
    static int frequencyToMidi(float frequency);

private:
    static std::string normalizeNoteName(const std::string& noteName);
    static float midiToFrequency(int midiNote);
    static const std::map<std::string, int> noteToSemitone;
    static const std::string semitoneToNote[12];
};

#endif
