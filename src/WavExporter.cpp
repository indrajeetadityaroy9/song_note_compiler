#include "WavExporter.h"
#include "AudioFile.h"
#include <iostream>

using namespace std;

/**
 * Export SoundSamples to WAV file format
 * @param samples - Pointer to SoundSamples object containing audio data
 * @param filename - Output filename (e.g., "output.wav")
 * @param bitDepth - Bit depth for WAV file (8, 16, 24, or 32), default is 16
 * @return true if export successful, false otherwise
 */
bool WavExporter::exportToWav(const SoundSamples* samples,
                              const string& filename,
                              int bitDepth) {

    // Validate input
    if (samples == nullptr) {
        cerr << "Error: samples pointer is null" << endl;
        return false;
    }

    if (samples->getLength() <= 0) {
        cerr << "Error: no samples to export" << endl;
        return false;
    }

    // Create AudioFile object for float samples
    AudioFile<float> audioFile;

    // Prepare audio buffer (mono channel)
    AudioFile<float>::AudioBuffer buffer;
    buffer.resize(1);  // 1 channel (mono)
    buffer[0].resize(samples->getLength());

    // Copy samples from SoundSamples to AudioFile buffer
    float* rawSamples = samples->getsamples();
    for (int i = 0; i < samples->getLength(); i++) {
        buffer[0][i] = rawSamples[i];
    }

    // Configure audio file properties
    audioFile.setAudioBuffer(buffer);
    audioFile.setSampleRate(static_cast<int>(samples->getSampleRate()));
    audioFile.setBitDepth(bitDepth);

    // Save to file
    bool success = audioFile.save(filename);

    if (!success) {
        cerr << "Error: failed to save WAV file: " << filename << endl;
        return false;
    }

    return true;
}
