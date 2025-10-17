#include "RhythmParser.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

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

    if (str.length() >= 2 && str[0] == '1' && str[1] == '6') {
        charsConsumed = 2;
        return 0.25f;  
    }

    charsConsumed = 1;
    char noteType = str[0];

    switch (noteType) {
        case 'w': case '1': return 4.0f;   
        case 'h': case '2': return 2.0f;   
        case 'q': case '4': return 1.0f;   
        case 'e': case '8': return 0.5f;   
        case 's':           return 0.25f;  
        default: return -1.0f;
    }
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