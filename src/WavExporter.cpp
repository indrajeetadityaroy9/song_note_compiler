#include "WavExporter.h"
#include "AudioFile.h"
#include <iostream>
using namespace std;

bool WavExporter::exportToWav(const SoundSamples* samples, const string& filename, int bitDepth) {
    if (samples == nullptr) {
        cerr << "Error: samples pointer is null" << endl;
        return false;
    }

    if (samples->getLength() <= 0) {
        cerr << "Error: no samples to export" << endl;
        return false;
    }

    AudioFile<float> audioFile;

    AudioFile<float>::AudioBuffer buffer;
    buffer.resize(1);
    buffer[0].resize(samples->getLength());

    float* rawSamples = samples->getsamples();
    for (int i = 0; i < samples->getLength(); i++) {
        buffer[0][i] = rawSamples[i];
    }

    audioFile.setAudioBuffer(buffer);
    audioFile.setSampleRate(static_cast<int>(samples->getSampleRate()));
    audioFile.setBitDepth(bitDepth);

    bool success = audioFile.save(filename);

    if (!success) {
        cerr << "Error: failed to save WAV file: " << filename << endl;
        return false;
    }

    return true;
}
