#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "WavExporter.h"
#include "soundio.h"
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

struct Note {
    float frequency;
    float duration;
};

void printUsage() {
    cout << "Usage: melody_generator <wave_type> <sample_rate> <output_file> [--wav]" << endl;
    cout << endl;
    cout << "This program generates the 'Twinkle Twinkle Little Star' melody." << endl;
    cout << endl;
    cout << "Arguments:" << endl;
    cout << "  wave_type    : Wave type (1=Sine, 2=Square, 3=Triangle, 4=Sawtooth)" << endl;
    cout << "  sample_rate  : Sample rate in Hz (e.g., 44100)" << endl;
    cout << "  output_file  : Output filename" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  --wav        : Output in WAV format (default: raw text)" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  melody_generator 1 44100 twinkle.wav --wav" << endl;
    cout << "  melody_generator 2 44100 twinkle.raw" << endl;
}

int main(int argc, char* argv[]) {

    // Check for help
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printUsage();
        return 0;
    }

    // Validate arguments
    if (argc < 4) {
        cout << "Error: Not enough arguments" << endl;
        printUsage();
        return 1;
    }

    // Check for --wav flag
    bool outputWav = false;
    if (argc >= 5 && strcmp(argv[4], "--wav") == 0) {
        outputWav = true;
    }

    // Parse arguments
    int waveType = atoi(argv[1]);
    float sampleRate = strtof(argv[2], nullptr);
    string outputFile = argv[3];

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

    cout << "Generating Twinkle Twinkle Little Star melody..." << endl;
    cout << "================================================" << endl;

    // Define the melody: Twinkle Twinkle Little Star
    // C C G G A A G - F F E E D D C
    // G G F F E E D - G G F F E E D
    // C C G G A A G - F F E E D D C

    vector<Note> melody = {
        // Line 1: "Twinkle twinkle little star"
        {261.63, 0.5}, {261.63, 0.5}, {392.00, 0.5}, {392.00, 0.5},
        {440.00, 0.5}, {440.00, 0.5}, {392.00, 1.0},

        // Line 2: "How I wonder what you are"
        {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5},
        {293.66, 0.5}, {293.66, 0.5}, {261.63, 1.0},

        // Line 3: "Up above the world so high"
        {392.00, 0.5}, {392.00, 0.5}, {349.23, 0.5}, {349.23, 0.5},
        {329.63, 0.5}, {329.63, 0.5}, {293.66, 1.0},

        // Line 4: "Like a diamond in the sky"
        {392.00, 0.5}, {392.00, 0.5}, {349.23, 0.5}, {349.23, 0.5},
        {329.63, 0.5}, {329.63, 0.5}, {293.66, 1.0},

        // Line 5: "Twinkle twinkle little star"
        {261.63, 0.5}, {261.63, 0.5}, {392.00, 0.5}, {392.00, 0.5},
        {440.00, 0.5}, {440.00, 0.5}, {392.00, 1.0},

        // Line 6: "How I wonder what you are"
        {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5},
        {293.66, 0.5}, {293.66, 0.5}, {261.63, 1.0}
    };

    // Generate first note
    SoundSamples* fullMelody = wave->generateSamples(melody[0].frequency, sampleRate, melody[0].duration);
    cout << "Generated note 1 of " << melody.size() << endl;

    // Concatenate remaining notes
    for (size_t i = 1; i < melody.size(); i++) {
        SoundSamples* note = wave->generateSamples(melody[i].frequency, sampleRate, melody[i].duration);
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
