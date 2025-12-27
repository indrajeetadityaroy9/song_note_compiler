#include "NoteParser.h"
#include "core/MusicXMLTypes.h"
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

const map<string, int> NoteParser::noteToSemitone = {
    {"C", 0}, {"C#", 1}, {"D", 2}, {"D#", 3}, {"E", 4}, {"F", 5},
    {"F#", 6}, {"G", 7}, {"G#", 8}, {"A", 9}, {"A#", 10}, {"B", 11}
};

// Reverse mapping: semitone to note name
const string NoteParser::semitoneToNote[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

float NoteParser::midiToFrequency(int midiNote) {
    if (midiNote < 0 || midiNote > 127) {
        return -1.0f;
    }
    return 440.0f * pow(2.0f, (midiNote - 69) / 12.0f);
}

string NoteParser::normalizeNoteName(const string& noteName) {
    string normalized;

    for (char c : noteName) {
        normalized += toupper(c);
    }

    if (normalized == "DB") normalized = "C#";
    else if (normalized == "EB") normalized = "D#";
    else if (normalized == "GB") normalized = "F#";
    else if (normalized == "AB") normalized = "G#";
    else if (normalized == "BB") normalized = "A#";

    return normalized;
}

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

    int midiNote = (octave + 1) * 12 + semitone;

    return midiToFrequency(midiNote);
}

float NoteParser::parseNote(const string& noteStr) {
    if (noteStr.empty()) {
        return -1.0f;
    }

    string str = noteStr;

    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    if (str.length() < 2) {
        return -1.0f;
    }

    char octaveChar = str.back();
    if (!isdigit(octaveChar)) {
        return -1.0f;
    }

    int octave = octaveChar - '0';

    string noteName = str.substr(0, str.length() - 1);

    return getNoteFrequency(noteName, octave);
}

bool NoteParser::isValidNote(const string& noteStr) {
    return parseNote(noteStr) > 0.0f;
}

// Convert note name (e.g., "C#4") to MusicXML pitch
MusicXMLPitch NoteParser::noteNameToPitch(const string& noteName) {
    MusicXMLPitch pitch;
    pitch.step = 'C';
    pitch.alter = 0;
    pitch.octave = 4;

    if (noteName.empty()) return pitch;

    string str = noteName;
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    if (str.length() < 2) return pitch;

    // Extract octave (last character)
    char octaveChar = str.back();
    if (!isdigit(octaveChar)) return pitch;
    pitch.octave = octaveChar - '0';

    // Extract note name and accidental
    string noteOnly = str.substr(0, str.length() - 1);
    for (char& c : noteOnly) c = toupper(c);

    pitch.step = noteOnly[0];
    pitch.alter = 0;

    if (noteOnly.length() > 1) {
        if (noteOnly[1] == '#') {
            pitch.alter = 1;
        } else if (noteOnly[1] == 'B') {
            // Handle flats - keep as flat for MusicXML (alter = -1)
            pitch.alter = -1;
        }
    }

    return pitch;
}

// Convert frequency to nearest MusicXML pitch
MusicXMLPitch NoteParser::frequencyToPitch(float frequency) {
    int midi = frequencyToMidi(frequency);
    string noteName = midiToNoteName(midi);
    return noteNameToPitch(noteName);
}

// Convert MusicXML pitch back to note name
string NoteParser::pitchToNoteName(const MusicXMLPitch& pitch) {
    string result;
    result += pitch.step;
    if (pitch.alter == 1) result += "#";
    else if (pitch.alter == -1) result += "b";
    result += to_string(pitch.octave);
    return result;
}

// Convert note name to MIDI number
int NoteParser::noteNameToMidi(const string& noteName) {
    if (noteName.empty()) return 60; // Default to middle C

    string str = noteName;
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

    if (str.length() < 2) return 60;

    char octaveChar = str.back();
    if (!isdigit(octaveChar)) return 60;
    int octave = octaveChar - '0';

    string noteOnly = str.substr(0, str.length() - 1);
    string normalized = normalizeNoteName(noteOnly);

    auto it = noteToSemitone.find(normalized);
    if (it == noteToSemitone.end()) return 60;

    return (octave + 1) * 12 + it->second;
}

// Convert MIDI number to note name
string NoteParser::midiToNoteName(int midiNote) {
    if (midiNote < 0 || midiNote > 127) return "C4";

    int octave = (midiNote / 12) - 1;
    int semitone = midiNote % 12;

    return semitoneToNote[semitone] + to_string(octave);
}

// Convert frequency to nearest MIDI number
int NoteParser::frequencyToMidi(float frequency) {
    if (frequency <= 0) return 69; // Default to A4
    // MIDI = 69 + 12 * log2(frequency / 440)
    return static_cast<int>(round(69.0f + 12.0f * log2f(frequency / 440.0f)));
}
