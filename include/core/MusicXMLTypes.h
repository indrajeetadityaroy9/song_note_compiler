#ifndef SONG_NOTE_COMPILER_MUSICXMLTYPES_H
#define SONG_NOTE_COMPILER_MUSICXMLTYPES_H

#include <string>

// MusicXML pitch representation
struct MusicXMLPitch {
    char step = 'C';    // 'C', 'D', 'E', 'F', 'G', 'A', 'B'
    int alter = 0;      // -1 = flat, 0 = natural, 1 = sharp
    int octave = 4;     // 0-8

    // Convert to note name string (e.g., "C4", "F#5", "Bb3")
    std::string toString() const {
        std::string result;
        result += step;
        if (alter == 1) result += "#";
        else if (alter == -1) result += "b";
        result += std::to_string(octave);
        return result;
    }
};

// MusicXML duration representation
struct MusicXMLDuration {
    int divisions = 0;      // Duration in divisions (quarter note = divisions value from attributes)
    std::string type;       // "whole", "half", "quarter", "eighth", "16th", "32nd"
    int dots = 0;           // Number of dots (0, 1, or 2)
    bool isTriplet = false; // Part of a triplet

    // Convert to beat count (quarter note = 1.0)
    float toBeats(int divisionsPerQuarter) const {
        if (divisionsPerQuarter <= 0) return 1.0f;
        return static_cast<float>(divisions) / divisionsPerQuarter;
    }
};

// Time signature representation
struct TimeSignature {
    int beats = 4;      // Numerator (e.g., 4 in 4/4)
    int beatType = 4;   // Denominator (e.g., 4 in 4/4)

    std::string toString() const {
        return std::to_string(beats) + "/" + std::to_string(beatType);
    }

    // Get beats per measure
    float beatsPerMeasure() const {
        // Convert to quarter note beats
        return static_cast<float>(beats) * (4.0f / beatType);
    }
};

#endif // SONG_NOTE_COMPILER_MUSICXMLTYPES_H
