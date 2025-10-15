#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "WavExporter.h"
#include "soundio.h"
#include "NoteParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

struct Note {
    string noteName;  // Note in letter notation (e.g., "C4", "A#4")
    float duration;   // Duration in seconds
};

void printUsage() {
    cout << "Usage: songgen <input_file> <wave_type> <sample_rate> <output_file> [--wav]" << endl;
    cout << endl;
    cout << "Song Note Compiler - Generates audio from musical notation files" << endl;
    cout << endl;
    cout << "Arguments:" << endl;
    cout << "  input_file   : Text file containing note sequence (NoteName Duration)" << endl;
    cout << "  wave_type    : Wave type (1=Sine, 2=Square, 3=Triangle, 4=Sawtooth)" << endl;
    cout << "  sample_rate  : Sample rate in Hz (e.g., 44100)" << endl;
    cout << "  output_file  : Output filename" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  --wav        : Output in WAV format (default: raw text)" << endl;
    cout << endl;
    cout << "Input file format:" << endl;
    cout << "  - One note per line: NoteName Duration" << endl;
    cout << "  - Lines starting with # are comments" << endl;
    cout << "  - Example: C4 0.5" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  songgen twinkle.txt 1 44100 output.wav --wav" << endl;
    cout << "  songgen song.txt 2 44100 output.raw" << endl;
}

/**
 * Read notes from a text file
 * Format: NoteName Duration (one note per line, # for comments)
 */
vector<Note> readNotesFromFile(const string& filename) {
    vector<Note> notes;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return notes;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Parse line
        istringstream iss(line);
        string noteName;
        float duration;

        if (iss >> noteName >> duration) {
            // Validate note
            if (!NoteParser::isValidNote(noteName)) {
                cerr << "Warning: Invalid note '" << noteName << "' on line " << lineNumber << ", skipping" << endl;
                continue;
            }

            // Validate duration
            if (duration <= 0) {
                cerr << "Warning: Invalid duration " << duration << " on line " << lineNumber << ", skipping" << endl;
                continue;
            }

            notes.push_back({noteName, duration});
        } else {
            cerr << "Warning: Could not parse line " << lineNumber << ": '" << line << "', skipping" << endl;
        }
    }

    file.close();
    return notes;
}

int main(int argc, char* argv[]) {

    // Check for help
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printUsage();
        return 0;
    }

    // Validate arguments
    if (argc < 5) {
        cout << "Error: Not enough arguments" << endl;
        printUsage();
        return 1;
    }

    // Check for --wav flag
    bool outputWav = false;
    if (argc >= 6 && strcmp(argv[5], "--wav") == 0) {
        outputWav = true;
    }

    // Parse arguments
    string inputFile = argv[1];
    int waveType = atoi(argv[2]);
    float sampleRate = strtof(argv[3], nullptr);
    string outputFile = argv[4];

    // Validate wave type
    if (waveType < 1 || waveType > 4) {
        cout << "Error: Invalid wave type. Must be 1-4" << endl;
        return 1;
    }

    // Validate sample rate
    if (sampleRate <= 0) {
        cout << "Error: Sample rate must be positive" << endl;
        return 1;
    }

    // Create wave generator
    Wave* wave = nullptr;
    switch(waveType) {
        case 1: wave = new SineWave("Sine"); break;
        case 2: wave = new SquareWave("Square"); break;
        case 3: wave = new TriangleWave("Triangle"); break;
        case 4: wave = new SawtoothWave("Sawtooth"); break;
    }

    // Read melody from input file
    cout << "Reading melody from '" << inputFile << "'..." << endl;
    cout << "================================================" << endl;

    vector<Note> melody = readNotesFromFile(inputFile);

    if (melody.empty()) {
        cout << "Error: No valid notes found in file '" << inputFile << "'" << endl;
        delete wave;
        return 1;
    }

    cout << "Loaded " << melody.size() << " notes from file" << endl;

    // Generate first note
    float firstFreq = NoteParser::parseNote(melody[0].noteName);
    if (firstFreq < 0) {
        cout << "Error: Invalid note '" << melody[0].noteName << "'" << endl;
        delete wave;
        return 1;
    }
    SoundSamples* fullMelody = wave->generateSamples(firstFreq, sampleRate, melody[0].duration);
    cout << "Generated note 1 of " << melody.size() << " (" << melody[0].noteName << " = " << firstFreq << " Hz)" << endl;

    // Concatenate remaining notes
    for (size_t i = 1; i < melody.size(); i++) {
        // Parse note name to frequency
        float frequency = NoteParser::parseNote(melody[i].noteName);
        if (frequency < 0) {
            cout << "Error: Invalid note '" << melody[i].noteName << "'" << endl;
            delete wave;
            delete fullMelody;
            return 1;
        }

        SoundSamples* note = wave->generateSamples(frequency, sampleRate, melody[i].duration);
        SoundSamples combined = *fullMelody + *note;

        delete fullMelody;
        delete note;

        fullMelody = new SoundSamples(combined);

        if ((i + 1) % 7 == 0 || i == melody.size() - 1) {
            cout << "Generated note " << (i + 1) << " of " << melody.size() << endl;
        }
    }

    cout << "================================================" << endl;
    cout << "Total duration: " << (fullMelody->getLength() / sampleRate) << " seconds" << endl;
    cout << "Total samples: " << fullMelody->getLength() << endl;

    // Export based on format
    if (outputWav) {
        bool success = WavExporter::exportToWav(fullMelody, outputFile, 16);
        if (success) {
            cout << "✓ WAV file generated: " << outputFile << endl;
            cout << "  Format: 16-bit PCM WAV, Mono, " << sampleRate << " Hz" << endl;
        } else {
            cout << "Error: Failed to generate WAV file" << endl;
            delete wave;
            delete fullMelody;
            return 1;
        }
    } else {
        SoundIO::OutputSound(fullMelody, outputFile);
        cout << "✓ Raw audio file generated: " << outputFile << endl;
    }

    // Cleanup
    delete wave;
    delete fullMelody;

    return 0;
}
