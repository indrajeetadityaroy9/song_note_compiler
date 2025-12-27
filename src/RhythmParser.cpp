#include "RhythmParser.h"
#include "core/MusicXMLTypes.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace std;

namespace {
    constexpr float DOTTED_MULTIPLIER = 1.5f;           
    constexpr float TRIPLET_MULTIPLIER = 2.0f / 3.0f;   
    constexpr float DEFAULT_TEMPO = 120.0f;
    constexpr float DEFAULT_TIME_SIG_BEATS = 4.0f;
}

float RhythmParser::getNoteBeatValue(char noteType) {
    switch (noteType) {
        case 'w': case '1': return 4.0f;   
        case 'h': case '2': return 2.0f;   
        case 'q': case '4': return 1.0f;   
        case 'e': case '8': return 0.5f;   
        case 's':           return 0.25f;  
        default: return -1.0f;
    }
}

float RhythmParser::beatsToSeconds(float beats, float tempo) {
    if (tempo <= 0.0f) tempo = DEFAULT_TEMPO;
    return beats * (60.0f / tempo);
}

float RhythmParser::parseTimeSignature(const string& timeSignature) {
    if (timeSignature.empty()) return DEFAULT_TIME_SIG_BEATS;

    size_t slashPos = timeSignature.find('/');
    if (slashPos == string::npos) return DEFAULT_TIME_SIG_BEATS;

    try {
        string numeratorStr = timeSignature.substr(0, slashPos);
        int numerator = stoi(numeratorStr);

        if (numerator <= 0) return DEFAULT_TIME_SIG_BEATS;
        return static_cast<float>(numerator);
    } catch (...) {
        return DEFAULT_TIME_SIG_BEATS;
    }
}

static bool isNumericDuration(const string& str) {
    if (str.empty()) return false;

    bool hasDecimal = false;
    for (char c : str) {
        if (c == '.') {
            if (hasDecimal) return false;  
            hasDecimal = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

static float parseBaseNoteType(const string& str, size_t& charsConsumed) {
    if (str.empty()) return -1.0f;

    // Handle "16" special case for sixteenth notes
    if (str.length() >= 2 && str[0] == '1' && str[1] == '6') {
        charsConsumed = 2;
        return 0.25f;
    }

    charsConsumed = 1;
    return RhythmParser::getNoteBeatValue(str[0]);
}

float RhythmParser::parseRhythm(const string& rhythmStr, float tempo, const string& timeSignature) {
    if (rhythmStr.empty()) return -1.0f;

    string str = rhythmStr;
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (isNumericDuration(str)) {
        try {
            float duration = stof(str);
            return (duration >= 0.0f) ? duration : -1.0f;
        } catch (...) {
            return -1.0f;
        }
    }

    size_t charsConsumed = 0;
    float beats = parseBaseNoteType(str, charsConsumed);

    if (beats < 0) return -1.0f;

    float multiplier = 1.0f;
    for (size_t i = charsConsumed; i < str.length(); i++) {
        switch (str[i]) {
            case '.':
                multiplier *= DOTTED_MULTIPLIER;
                break;
            case 't':
                multiplier *= TRIPLET_MULTIPLIER;
                break;
            default:
                return -1.0f;
        }
    }

    beats *= multiplier;
    return beatsToSeconds(beats, tempo);
}

// Convert duration string to MusicXML duration structure
MusicXMLDuration RhythmParser::durationStrToMusicXML(const string& durationStr, int divisions) {
    MusicXMLDuration result;
    result.divisions = divisions;  // Default to quarter note
    result.type = "quarter";
    result.dots = 0;
    result.isTriplet = false;

    if (durationStr.empty()) return result;

    string str = durationStr;
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    // Parse base note type
    float baseBeats = 0.0f;
    size_t charsConsumed = 0;

    // Handle "16" for sixteenth notes
    if (str.length() >= 2 && str[0] == '1' && str[1] == '6') {
        baseBeats = 0.25f;
        charsConsumed = 2;
    } else {
        baseBeats = getNoteBeatValue(str[0]);
        charsConsumed = 1;
    }

    if (baseBeats <= 0) {
        // Try to parse as numeric (direct seconds) - default to quarter note
        return result;
    }

    // Set type string based on base beats
    result.type = beatsToTypeString(baseBeats);

    // Parse modifiers
    float totalBeats = baseBeats;
    for (size_t i = charsConsumed; i < str.length(); i++) {
        if (str[i] == '.') {
            result.dots++;
            totalBeats *= DOTTED_MULTIPLIER;
        } else if (str[i] == 't') {
            result.isTriplet = true;
            totalBeats *= TRIPLET_MULTIPLIER;
        }
    }

    // Calculate MusicXML duration value
    result.divisions = static_cast<int>(round(totalBeats * divisions));

    return result;
}

// Convert MusicXML duration back to duration string
string RhythmParser::musicXMLToDurationStr(const MusicXMLDuration& mxmlDur) {
    string result;

    // Convert type to our notation
    float beats = typeStringToBeats(mxmlDur.type);

    // Map beats to duration character
    if (beats >= 4.0f) result = "w";
    else if (beats >= 2.0f) result = "h";
    else if (beats >= 1.0f) result = "q";
    else if (beats >= 0.5f) result = "e";
    else result = "s";

    // Add dots
    for (int i = 0; i < mxmlDur.dots; i++) {
        result += ".";
    }

    // Add triplet marker
    if (mxmlDur.isTriplet) {
        result += "t";
    }

    return result;
}

// Calculate optimal divisions for a set of durations
int RhythmParser::calculateDivisions(const vector<string>& durationStrs) {
    // Use 24 divisions per quarter note as default
    // This handles: whole (96), half (48), quarter (24), eighth (12),
    // sixteenth (6), triplets (8), dotted notes cleanly
    return DEFAULT_DIVISIONS;
}

// Convert beats to MusicXML duration units
int RhythmParser::beatsToMusicXMLDuration(float beats, int divisions) {
    return static_cast<int>(round(beats * divisions));
}

// Convert MusicXML duration to beats
float RhythmParser::musicXMLDurationToBeats(int duration, int divisions) {
    if (divisions <= 0) return 1.0f;
    return static_cast<float>(duration) / divisions;
}

// Convert beat value to MusicXML type string
string RhythmParser::beatsToTypeString(float beats) {
    if (beats >= 4.0f) return "whole";
    if (beats >= 2.0f) return "half";
    if (beats >= 1.0f) return "quarter";
    if (beats >= 0.5f) return "eighth";
    if (beats >= 0.25f) return "16th";
    return "32nd";
}

// Convert MusicXML type string to beats
float RhythmParser::typeStringToBeats(const string& type) {
    if (type == "whole" || type == "breve") return 4.0f;
    if (type == "half") return 2.0f;
    if (type == "quarter") return 1.0f;
    if (type == "eighth") return 0.5f;
    if (type == "16th") return 0.25f;
    if (type == "32nd") return 0.125f;
    return 1.0f;  // Default to quarter
}