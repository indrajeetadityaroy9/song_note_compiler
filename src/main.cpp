#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "soundio.h"
#include "WavExporter.h"
#include <cstring>
#include <iostream>

using namespace std;

void printUsage() {
    cout << "Usage: genwave <wave_type> <sample_rate> <frequency> <duration> <output_file> [--wav]" << endl;
    cout << endl;
    cout << "Arguments:" << endl;
    cout << "  wave_type    : Wave type (1=Sine, 2=Square, 3=Triangle, 4=Sawtooth)" << endl;
    cout << "  sample_rate  : Sample rate in Hz (e.g., 44100)" << endl;
    cout << "  frequency    : Frequency in Hz (e.g., 440 for A4)" << endl;
    cout << "  duration     : Duration in seconds (e.g., 1.0)" << endl;
    cout << "  output_file  : Output filename (extension determines format)" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  --wav        : Output in WAV format (default: raw text)" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  genwave 1 44100 440 1.0 output.raw         # Sine wave, raw text format" << endl;
    cout << "  genwave 1 44100 440 1.0 output.wav --wav   # Sine wave, WAV format" << endl;
}

int main(int argc, char *argv[]) {

    // Check if help requested
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printUsage();
        return 0;
    }

    // Validate argument count
    if (argc < 6) {
        cout << "Error: Not enough arguments" << endl;
        printUsage();
        return 1;
    }

    // Check for optional --wav flag
    bool outputWav = false;
    if (argc >= 7 && strcmp(argv[6], "--wav") == 0) {
        outputWav = true;
    }

    // Parse wave type
    Wave *w = nullptr;
    if (strchr(argv[1], '1')) {
        w = new SineWave("SineWave");
    } else if (strchr(argv[1], '2')) {
        w = new SquareWave("SquareWave");
    } else if (strchr(argv[1], '3')) {
        w = new TriangleWave("TriangleWave");
    } else if (strchr(argv[1], '4')) {
        w = new SawtoothWave("SawtoothWave");
    } else {
        cout << "Error: Invalid wave type. Must be 1, 2, 3, or 4" << endl;
        printUsage();
        return 1;
    }

    // Parse audio parameters
    float sample_rate = strtof(argv[2], nullptr);
    float frequency = strtof(argv[3], nullptr);
    float duration = strtof(argv[4], nullptr);
    string filename = argv[5];

    // Validate parameters
    if (sample_rate <= 0 || frequency <= 0 || duration <= 0) {
        cout << "Error: sample_rate, frequency, and duration must be positive values" << endl;
        delete w;
        return 1;
    }

    // Generate samples
    SoundSamples *s = w->generateSamples(frequency, sample_rate, duration);

    // Output based on format
    if (outputWav) {
        // Export to WAV format
        bool success = WavExporter::exportToWav(s, filename, 16);
        if (success) {
            cout << "WAV file generated: " << filename << endl;
        } else {
            cout << "Error: Failed to generate WAV file" << endl;
            delete w;
            delete s;
            return 1;
        }
    } else {
        // Export to raw text format (original behavior)
        SoundIO::OutputSound(s, filename);
        cout << "Raw audio file generated: " << filename << endl;
    }

    // Cleanup
    delete w;
    delete s;

    return 0;
}
