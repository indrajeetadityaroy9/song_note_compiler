#include "NoteParser.h"
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

const map<string, int> NoteParser::noteToSemitone = {
    {"C", 0}, {"C#", 1}, {"D", 2}, {"D#", 3}, {"E", 4}, {"F", 5},
    {"F#", 6}, {"G", 7}, {"G#", 8}, {"A", 9}, {"A#", 10}, {"B", 11}
};

/**
 * Calculate frequency from MIDI note number
 * Formula: f = 440 * 2^((n-69)/12) where n is MIDI note number
 * A4 = MIDI 69 = 440 Hz (reference pitch)
 */
float NoteParser::midiToFrequency(int midiNote) {
    if (midiNote < 0 || midiNote > 127) {
        return -1.0f;
    }
    return 440.0f * pow(2.0f, (midiNote - 69) / 12.0f);
}

/**
 * Normalize note name (convert flats to sharps, uppercase)
 */
string NoteParser::normalizeNoteName(const string& noteName) {
    string normalized;

    for (char c : noteName) {
        normalized += toupper(c);
    }

    // Convert flats to equivalent sharps
    if (normalized == "DB") normalized = "C#";
    else if (normalized == "EB") normalized = "D#";
    else if (normalized == "GB") normalized = "F#";
    else if (normalized == "AB") normalized = "G#";
    else if (normalized == "BB") normalized = "A#";

    return normalized;
}

/**
 * Get frequency for a note name and octave
 */
float NoteParser::getNoteFrequency(const string& noteName, int octave) {
    if (octave < 0 || octave > 8) {
        return -1.0f;
    }

    string normalizedNote = normalizeNoteName(noteName);

    auto it = noteToSemitone.find(normalizedNote);
    if (it == noteToSemitone.end()) {
        return -1.0f;
    }

    int semitone = it->second;

    // Calculate MIDI note number: (octave + 1) * 12 + semitone
    // C4 = (4+1)*12 + 0 = 60
    // A4 = (4+1)*12 + 9 = 69
    int midiNote = (octave + 1) * 12 + semitone;

    return midiToFrequency(midiNote);
}

/**
 * Parse a note string (e.g., "C4", "A#4", "Bb3") and return its frequency in Hz
 */
float NoteParser::parseNote(const string& noteStr) {
    if (noteStr.empty()) {
        return -1.0f;
    }

    string str = noteStr;

    // Remove whitespace
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    if (str.length() < 2) {
        return -1.0f;
    }

    // Extract octave (last character)
    char octaveChar = str.back();
    if (!isdigit(octaveChar)) {
        return -1.0f;
    }

    int octave = octaveChar - '0';

    // Extract note name (everything except last character)
    string noteName = str.substr(0, str.length() - 1);

    return getNoteFrequency(noteName, octave);
}

/**
 * Check if a string is a valid note
 */
bool NoteParser::isValidNote(const string& noteStr) {
    return parseNote(noteStr) > 0.0f;
}
