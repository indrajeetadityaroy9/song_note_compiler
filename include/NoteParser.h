#ifndef SONG_NOTE_COMPILER_NOTEPARSER_H
#define SONG_NOTE_COMPILER_NOTEPARSER_H

#include <string>
#include <map>

class NoteParser {
public:
    /**
     * Parse a note string (e.g., "C4", "A#4", "Bb3") and return its frequency in Hz
     * @param noteStr - Note string in format: [A-G][#/b]?[0-8]
     * @return frequency in Hz, or -1.0 if parsing fails
     *
     * Examples:
     *   "C4" -> 261.63
     *   "A4" -> 440.00
     *   "C#4" or "Db4" -> 277.18
     */
    static float parseNote(const std::string& noteStr);

    /**
     * Get frequency for a note name and octave
     * @param noteName - Note name (C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B)
     * @param octave - Octave number (0-8)
     * @return frequency in Hz, or -1.0 if invalid
     */
    static float getNoteFrequency(const std::string& noteName, int octave);

    /**
     * Check if a string is a valid note
     * @param noteStr - Note string to validate
     * @return true if valid, false otherwise
     */
    static bool isValidNote(const std::string& noteStr);

private:
    /**
     * Normalize note name (convert flats to sharps, uppercase)
     * @param noteName - Raw note name
     * @return normalized note name
     */
    static std::string normalizeNoteName(const std::string& noteName);

    /**
     * Calculate frequency from MIDI note number
     * Formula: f = 440 * 2^((n-69)/12) where n is MIDI note number
     * @param midiNote - MIDI note number (0-127)
     * @return frequency in Hz
     */
    static float midiToFrequency(int midiNote);

    // Map of note names to semitones above C (C=0, C#=1, D=2, etc.)
    static const std::map<std::string, int> noteToSemitone;
};

#endif //SONG_NOTE_COMPILER_NOTEPARSER_H
