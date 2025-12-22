#include "SineWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "WavExporter.h"
#include "soundio.h"
#include "NoteParser.h"
#include "RhythmParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <map>
using namespace std;

struct Note {
    vector<string> noteNames;
    string durationStr;
    float duration;
    float volume;
    int trackId;
};

struct Track {
    int id;
    int waveType;
    float volume;
    string name;
};

// Encapsulates all song parsing state (replaces global variables)
class SongContext {
public:
    float tempo = 120.0f;
    string timeSignature = "4/4";
    map<int, Track> tracks;
    int currentTrack = 1;

    void setTempo(float t) { tempo = t; }
    void setTimeSignature(const string& ts) { timeSignature = ts; }

    void defineTrack(int id, int waveType, float vol) {
        vol = std::clamp(vol, 0.0f, 1.0f);
        tracks[id] = {id, waveType, vol, "Track " + to_string(id)};
    }

    bool switchTrack(int trackId) {
        if (tracks.find(trackId) != tracks.end()) {
            currentTrack = trackId;
            return true;
        }
        return false;
    }

    Track getTrackInfo(int trackId, int defaultWaveType) const {
        auto it = tracks.find(trackId);
        if (it != tracks.end()) {
            return it->second;
        }
        return {trackId, defaultWaveType, 1.0f, "Default Track " + to_string(trackId)};
    }
};  


bool isChord(const string& noteStr) {
    return noteStr.find(',') != string::npos;
}


vector<string> parseChord(const string& chordStr) {
    vector<string> notes;
    istringstream iss(chordStr);
    string note;
    
    while (getline(iss, note, ',')) {
        
        note.erase(remove_if(note.begin(), note.end(), ::isspace), note.end());
        if (!note.empty()) {
            notes.push_back(note);
        }
    }
    
    return notes;
}

// Helper function to generate a note directly into a buffer at a given position
void generateNoteIntoBuffer(float* buffer, int writePos, Wave* wave,
                            float frequency, float sampleRate, float duration, float volume) {
    int length = static_cast<int>(sampleRate * duration);
    for (int i = 0; i < length; i++) {
        float sample = wave->generateFunction(i * frequency / sampleRate);
        buffer[writePos + i] += sample * volume;
    }
}

void printUsage() {
    cout << "Usage: songgen <input_file> <wave_type> <sample_rate> <output_file> [--wav]" << endl;
    cout << endl;
    cout << "Song Note Compiler - Generates audio from musical notation files" << endl;
    cout << endl;
    cout << "Arguments:" << endl;
    cout << "  input_file   : Text file containing note sequence (NoteName Duration [Volume])" << endl;
    cout << "  wave_type    : Wave type (1=Sine, 2=Square, 3=Triangle, 4=Sawtooth)" << endl;
    cout << "  sample_rate  : Sample rate in Hz (e.g., 44100)" << endl;
    cout << "  output_file  : Output filename" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  --wav        : Output in WAV format (default: raw text)" << endl;
    cout << endl;
    cout << "Input file format:" << endl;
    cout << "  - One note/chord per line: NoteName Duration [Volume]" << endl;
    cout << "  - Lines starting with # are comments" << endl;
    cout << "  - Single notes: C4 0.5 or C4 q" << endl;
    cout << "  - Chords: C4,E4,G4 0.5 or C4,E4,G4 q" << endl;
    cout << "  - Rests: R 0.5 or R q" << endl;
    cout << "  - Volume: Optional third parameter 0.0-1.0 (default: 1.0)" << endl;
    cout << "  - Tempo changes: TEMPO 120 (sets tempo to 120 BPM)" << endl;
    cout << "  - Time signature: TIME 4/4 (sets time signature to 4/4)" << endl;
    cout << "  - Track definitions: TRACK <track_id> <wave_type> <volume>" << endl;
    cout << "  - Track switching: > <track_id>" << endl;
    cout << endl;
    cout << "Duration formats:" << endl;
    cout << "  - Numeric: 0.5 (seconds)" << endl;
    cout << "  - Rhythmic notation:" << endl;
    cout << "    w  = whole note (4 beats)" << endl;
    cout << "    h  = half note (2 beats)" << endl;
    cout << "    q  = quarter note (1 beat)" << endl;
    cout << "    e  = eighth note (1/2 beat)" << endl;
    cout << "    s  = sixteenth note (1/4 beat)" << endl;
    cout << "    .  = dotted (increases duration by 50%)" << endl;
    cout << "    t  = triplet (decreases duration to 2/3)" << endl;
    cout << "  - Examples: q (quarter note), h. (dotted half note), 8t (eighth note triplet)" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "  songgen twinkle.txt 1 44100 output.wav --wav" << endl;
    cout << "  songgen song.txt 2 44100 output.raw" << endl;
}

vector<Note> readNotesFromFile(const string& filename, SongContext& ctx) {
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

        if (line.empty() || line[0] == '#') {
            continue;
        }

        istringstream iss(line);
        string command;
        string noteStr;
        string durationStr;
        float volume = 1.0f;

        if (iss >> command) {

            if (command == "TRACK") {
                int trackId, waveType;
                float trackVolume = 1.0f;
                if (iss >> trackId >> waveType >> trackVolume) {
                    if (waveType < 1 || waveType > 4) {
                        cerr << "Warning: Invalid wave type " << waveType << " on line " << lineNumber << ", skipping" << endl;
                        continue;
                    }
                    ctx.defineTrack(trackId, waveType, trackVolume);
                    cout << "Defined track " << trackId << " (wave type: " << waveType << ", volume: " << trackVolume << ")" << endl;
                } else {
                    cerr << "Warning: Invalid TRACK definition on line " << lineNumber << ", skipping" << endl;
                }
                continue;
            }

            if (command == ">") {
                int trackId;
                if (iss >> trackId) {
                    if (ctx.switchTrack(trackId)) {
                        cout << "Switched to track " << trackId << endl;
                    } else {
                        cerr << "Warning: Undefined track " << trackId << " on line " << lineNumber << ", using current track" << endl;
                    }
                } else {
                    cerr << "Warning: Invalid track switch on line " << lineNumber << ", skipping" << endl;
                }
                continue;
            }

            if (command.substr(0, 5) == "TEMPO") {
                string tempoStr;
                iss >> tempoStr;
                try {
                    ctx.setTempo(stof(tempoStr));
                    cout << "Set tempo to " << ctx.tempo << " BPM" << endl;
                    continue;
                } catch (...) {
                    cerr << "Warning: Invalid tempo value on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
            }

            if (command.substr(0, 4) == "TIME") {
                string timeSigStr;
                iss >> timeSigStr;
                ctx.setTimeSignature(timeSigStr);
                cout << "Set time signature to " << ctx.timeSignature << endl;
                continue;
            }

            noteStr = command;

            if (!(iss >> durationStr)) {
                cerr << "Warning: Could not parse duration on line " << lineNumber << ": '" << line << "', skipping" << endl;
                continue;
            }

            iss >> volume;

            if (noteStr == "R" || noteStr == "r") {
                float duration = RhythmParser::parseRhythm(durationStr, ctx.tempo, ctx.timeSignature);
                if (duration < 0) {
                    cerr << "Warning: Invalid duration '" << durationStr << "' on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
                notes.push_back({{}, durationStr, duration, volume, ctx.currentTrack});
                continue;
            }

            vector<string> noteNames;
            if (isChord(noteStr)) {
                noteNames = parseChord(noteStr);

                bool validChord = true;
                for (const string& note : noteNames) {
                    if (!NoteParser::isValidNote(note)) {
                        cerr << "Warning: Invalid note '" << note << "' in chord on line " << lineNumber << ", skipping line" << endl;
                        validChord = false;
                        break;
                    }
                }

                if (!validChord) {
                    continue;
                }
            } else {
                if (!NoteParser::isValidNote(noteStr)) {
                    cerr << "Warning: Invalid note '" << noteStr << "' on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
                noteNames.push_back(noteStr);
            }

            float duration = RhythmParser::parseRhythm(durationStr, ctx.tempo, ctx.timeSignature);
            if (duration < 0) {
                cerr << "Warning: Invalid duration '" << durationStr << "' on line " << lineNumber << ", skipping" << endl;
                continue;
            }

            volume = std::clamp(volume, 0.0f, 1.0f);

            notes.push_back({noteNames, durationStr, duration, volume, ctx.currentTrack});
        } else {
            cerr << "Warning: Could not parse line " << lineNumber << ": '" << line << "', skipping" << endl;
        }
    }

    file.close();
    return notes;
}

SoundSamples* generateTrackAudio(const vector<Note>& trackMelody, const Track& trackInfo, float sampleRate) {

    Wave* wave = nullptr;
    switch(trackInfo.waveType) {
        case 1: wave = new SineWave("Sine"); break;
        case 2: wave = new SquareWave("Square"); break;
        case 3: wave = new TriangleWave("Triangle"); break;
        case 4: wave = new SawtoothWave("Sawtooth"); break;
        default: wave = new SineWave("Sine"); break;
    }

    cout << "  Generating audio for track " << trackInfo.id << " (" << trackInfo.name << ")..." << endl;

    // Pre-calculate total samples needed (O(n) instead of O(n²))
    int totalSamples = 0;
    for (const Note& note : trackMelody) {
        totalSamples += static_cast<int>(sampleRate * note.duration);
    }

    // Allocate full buffer once
    SoundSamples* trackFullMelody = new SoundSamples(totalSamples, sampleRate);
    float* buffer = trackFullMelody->getsamples();
    int writePosition = 0;

    // Process each note/chord/rest
    for (size_t i = 0; i < trackMelody.size(); i++) {
        const Note& note = trackMelody[i];
        int segmentLength = static_cast<int>(sampleRate * note.duration);

        if (note.noteNames.empty()) {
            // Rest - buffer already zero-initialized, just log it
            cout << "  Generated rest " << (i + 1) << " of " << trackMelody.size()
                 << " (duration: " << note.duration << "s)" << endl;
        } else {
            // Note or chord
            float finalVolume = trackInfo.volume * note.volume;
            float noteVolume = finalVolume / note.noteNames.size();

            // Cache frequencies to avoid redundant parseNote calls
            vector<float> frequencies;
            frequencies.reserve(note.noteNames.size());

            for (const string& noteName : note.noteNames) {
                float frequency = NoteParser::parseNote(noteName);
                if (frequency < 0) {
                    cout << "  Error: Invalid note '" << noteName << "'" << endl;
                    delete wave;
                    delete trackFullMelody;
                    return nullptr;
                }
                frequencies.push_back(frequency);
                generateNoteIntoBuffer(buffer, writePosition, wave, frequency,
                                       sampleRate, note.duration, noteVolume);
            }

            // Log progress
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
            cout << "  Processed " << (i + 1) << " of " << trackMelody.size() << " notes/chords" << endl;
        }
    }

    delete wave;

    cout << "  Finished generating audio for track " << trackInfo.id << endl;
    return trackFullMelody;
}

int main(int argc, char* argv[]) {

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printUsage();
        return 0;
    }

    if (argc < 5) {
        cout << "Error: Not enough arguments" << endl;
        printUsage();
        return 1;
    }

    bool outputWav = false;
    if (argc >= 6 && strcmp(argv[5], "--wav") == 0) {
        outputWav = true;
    }

    string inputFile = argv[1];
    int waveType = atoi(argv[2]);
    float sampleRate = strtof(argv[3], nullptr);
    string outputFile = argv[4];

    if (waveType < 1 || waveType > 4) {
        cout << "Error: Invalid wave type. Must be 1-4" << endl;
        return 1;
    }

    if (sampleRate <= 0) {
        cout << "Error: Sample rate must be positive" << endl;
        return 1;
    }

    cout << "Reading melody from '" << inputFile << "'..." << endl;
    cout << "================================================" << endl;

    SongContext ctx;
    vector<Note> melody = readNotesFromFile(inputFile, ctx);

    if (melody.empty()) {
        cout << "Error: No valid notes found in file '" << inputFile << "'" << endl;
        return 1;
    }

    cout << "Loaded " << melody.size() << " notes/chords from file" << endl;

    map<int, vector<Note>> tracksNotes;
    for (const Note& note : melody) {
        tracksNotes[note.trackId].push_back(note);
    }

    map<int, SoundSamples*> trackAudio;
    int totalDuration = 0;

    for (auto& pair : tracksNotes) {
        int trackId = pair.first;
        vector<Note>& trackMelody = pair.second;

        Track trackInfo = ctx.getTrackInfo(trackId, waveType);

        SoundSamples* trackSamples = generateTrackAudio(trackMelody, trackInfo, sampleRate);
        if (trackSamples != nullptr) {
            trackAudio[trackId] = trackSamples;

            if (trackSamples->getLength() > totalDuration) {
                totalDuration = trackSamples->getLength();
            }
        }
    }

    cout << "Mixing " << trackAudio.size() << " tracks..." << endl;
    SoundSamples* fullMelody = new SoundSamples(totalDuration, sampleRate);

    for (auto& pair : trackAudio) {
        SoundSamples* trackSamples = pair.second;
        float* trackData = trackSamples->getsamples();
        float* fullData = fullMelody->getsamples();

        for (int i = 0; i < trackSamples->getLength(); i++) {
            fullData[i] = std::clamp(fullData[i] + trackData[i], -1.0f, 1.0f);
        }

        delete trackSamples;
    }

    cout << "================================================" << endl;
    cout << "Total duration: " << (fullMelody->getLength() / sampleRate) << " seconds" << endl;
    cout << "Total samples: " << fullMelody->getLength() << endl;

    if (outputWav) {
        bool success = WavExporter::exportToWav(fullMelody, outputFile, 16);
        if (success) {
            cout << "WAV file generated: " << outputFile << endl;
            cout << "  Format: 16-bit PCM WAV, Mono, " << sampleRate << " Hz" << endl;
        } else {
            cout << "Error: Failed to generate WAV file" << endl;
            delete fullMelody;
            return 1;
        }
    } else {
        SoundIO::OutputSound(fullMelody, outputFile);
        cout << "Raw audio file generated: " << outputFile << endl;
    }

    delete fullMelody;

    return 0;
}