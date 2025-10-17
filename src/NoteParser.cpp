#include "NoteParser.h"
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

const map<string, int> NoteParser::noteToSemitone = {
    {"C", 0}, {"C#", 1}, {"D", 2}, {"D#", 3}, {"E", 4}, {"F", 5},
    {"F#", 6}, {"G", 7}, {"G#", 8}, {"A", 9}, {"A#", 10}, {"B", 11}
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
