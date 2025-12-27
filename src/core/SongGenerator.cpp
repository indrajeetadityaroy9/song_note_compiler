#include "core/SongGenerator.h"
#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "NoteParser.h"
#include "Envelope.h"
#include <iostream>
#include <algorithm>

using namespace std;

// Default envelope parameters for natural-sounding notes
static const EnvelopeParams defaultEnvelope = {
    0.01f,   // Attack: 10ms
    0.05f,   // Decay: 50ms
    0.7f,    // Sustain: 70%
    0.08f    // Release: 80ms
};

unique_ptr<Wave> SongGenerator::createWave(int waveType) {
    switch(waveType) {
        case 1: return make_unique<SineWave>("Sine");
        case 2: return make_unique<SquareWave>("Square");
        case 3: return make_unique<TriangleWave>("Triangle");
        case 4: return make_unique<SawtoothWave>("Sawtooth");
        default: return make_unique<SineWave>("Sine");
    }
}

void SongGenerator::generateNoteIntoBuffer(float* buffer, int writePos, Wave* wave,
                                           float frequency, float sampleRate,
                                           float duration, float volume) {
    int length = static_cast<int>(sampleRate * duration);

    // Use phase accumulation instead of computing i * frequency / sampleRate each iteration
    float phaseIncrement = frequency / sampleRate;
    float phase = 0.0f;

    for (int i = 0; i < length; i++) {
        float sample = wave->generateFunction(phase);
        buffer[writePos + i] += sample * volume;
        phase += phaseIncrement;
    }
}

unique_ptr<SoundSamples> SongGenerator::generateTrackAudio(const vector<Note>& trackMelody,
                                                            const Track& trackInfo,
                                                            float sampleRate) {
    auto wave = createWave(trackInfo.waveType);

    cout << "  Generating audio for track " << trackInfo.id
         << " (" << trackInfo.name << ")..." << endl;

    // Pre-calculate total samples needed
    int totalSamples = 0;
    for (const Note& note : trackMelody) {
        totalSamples += static_cast<int>(sampleRate * note.duration);
    }

    // Allocate full buffer once
    auto trackFullMelody = make_unique<SoundSamples>(totalSamples, sampleRate);
    float* buffer = trackFullMelody->getsamples();
    int writePosition = 0;

    // Process each note/chord/rest
    for (size_t i = 0; i < trackMelody.size(); i++) {
        const Note& note = trackMelody[i];
        int segmentLength = static_cast<int>(sampleRate * note.duration);

        if (note.noteNames.empty() ||
            (note.noteNames.size() == 1 && (note.noteNames[0] == "R" || note.noteNames[0] == "r"))) {
            // Rest - buffer already zero-initialized
            cout << "  Generated rest " << (i + 1) << " of " << trackMelody.size()
                 << " (duration: " << note.duration << "s)" << endl;
        } else {
            float finalVolume = trackInfo.volume * note.volume;
            float noteVolume = finalVolume / note.noteNames.size();

            vector<float> frequencies;
            frequencies.reserve(note.noteNames.size());

            for (const string& noteName : note.noteNames) {
                float frequency = NoteParser::parseNote(noteName);
                if (frequency < 0) {
                    cout << "  Error: Invalid note '" << noteName << "'" << endl;
                    return nullptr;  // unique_ptr automatically cleans up
                }
                frequencies.push_back(frequency);
                generateNoteIntoBuffer(buffer, writePosition, wave.get(), frequency,
                                       sampleRate, note.duration, noteVolume);
            }

            // Apply ADSR envelope to this note/chord segment
            Envelope::apply(buffer + writePosition, segmentLength, sampleRate, defaultEnvelope);

            if (note.noteNames.size() == 1) {
                cout << "  Generated note " << (i + 1) << " of " << trackMelody.size()
                     << " (" << note.noteNames[0] << " = " << frequencies[0] << " Hz)" << endl;
            } else {
                cout << "  Generated chord " << (i + 1) << " of " << trackMelody.size() << " (";
                for (size_t j = 0; j < note.noteNames.size(); j++) {
                    if (j > 0) cout << ", ";
                    cout << note.noteNames[j] << " = " << frequencies[j] << " Hz";
                }
                cout << ")" << endl;
            }
        }

        writePosition += segmentLength;

        if ((i + 1) % 7 == 0 || i == trackMelody.size() - 1) {
            cout << "  Processed " << (i + 1) << " of " << trackMelody.size()
                 << " notes/chords" << endl;
        }
    }

    cout << "  Finished generating audio for track " << trackInfo.id << endl;
    return trackFullMelody;
}

unique_ptr<SoundSamples> SongGenerator::mixTracks(const map<int, SoundSamples*>& trackAudio,
                                                   float sampleRate) {
    // Find the longest track
    int totalDuration = 0;
    for (const auto& pair : trackAudio) {
        if (pair.second->getLength() > totalDuration) {
            totalDuration = pair.second->getLength();
        }
    }

    cout << "Mixing " << trackAudio.size() << " tracks..." << endl;
    auto fullMelody = make_unique<SoundSamples>(totalDuration, sampleRate);

    for (const auto& pair : trackAudio) {
        SoundSamples* trackSamples = pair.second;
        float* trackData = trackSamples->getsamples();
        float* fullData = fullMelody->getsamples();

        for (int i = 0; i < trackSamples->getLength(); i++) {
            fullData[i] = std::clamp(fullData[i] + trackData[i], -1.0f, 1.0f);
        }
    }

    return fullMelody;
}
