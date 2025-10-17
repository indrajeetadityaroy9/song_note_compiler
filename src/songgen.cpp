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


float globalTempo = 120.0f;
string globalTimeSignature = "4/4";


map<int, Track> tracks;
int currentTrack = 1;  


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
                    
                    
                    if (trackVolume < 0.0f) trackVolume = 0.0f;
                    if (trackVolume > 1.0f) trackVolume = 1.0f;
                    
                    
                    tracks[trackId] = {trackId, waveType, trackVolume, "Track " + to_string(trackId)};
                    cout << "Defined track " << trackId << " (wave type: " << waveType << ", volume: " << trackVolume << ")" << endl;
                } else {
                    cerr << "Warning: Invalid TRACK definition on line " << lineNumber << ", skipping" << endl;
                }
                continue;
            }
            
            
            if (command == ">") {
                int trackId;
                if (iss >> trackId) {
                    if (tracks.find(trackId) != tracks.end()) {
                        currentTrack = trackId;
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
                    globalTempo = stof(tempoStr);
                    cout << "Set tempo to " << globalTempo << " BPM" << endl;
                    continue;
                } catch (...) {
                    cerr << "Warning: Invalid tempo value on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
            }
            
            
            if (command.substr(0, 4) == "TIME") {
                string timeSigStr;
                iss >> timeSigStr;
                globalTimeSignature = timeSigStr;
                cout << "Set time signature to " << globalTimeSignature << endl;
                continue;
            }
            
            
            noteStr = command;
            
            
            if (!(iss >> durationStr)) {
                cerr << "Warning: Could not parse duration on line " << lineNumber << ": '" << line << "', skipping" << endl;
                continue;
            }
            
            
            iss >> volume;
            
            
            if (noteStr == "R" || noteStr == "r") {
                float duration = RhythmParser::parseRhythm(durationStr, globalTempo, globalTimeSignature);
                if (duration < 0) {
                    cerr << "Warning: Invalid duration '" << durationStr << "' on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
                notes.push_back({{}, durationStr, duration, volume, currentTrack});  
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

            
            float duration = RhythmParser::parseRhythm(durationStr, globalTempo, globalTimeSignature);
            if (duration < 0) {
                cerr << "Warning: Invalid duration '" << durationStr << "' on line " << lineNumber << ", skipping" << endl;
                continue;
            }

            
            if (volume < 0.0f) volume = 0.0f;
            if (volume > 1.0f) volume = 1.0f;

            notes.push_back({noteNames, durationStr, duration, volume, currentTrack});
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

    
    SoundSamples* trackFullMelody = nullptr;
    if (trackMelody[0].noteNames.empty()) {
        
        trackFullMelody = new SoundSamples(static_cast<int>(sampleRate * trackMelody[0].duration), sampleRate);
        cout << "  Generated rest 1 of " << trackMelody.size() << " (duration: " << trackMelody[0].duration << "s)" << endl;
    } else if (trackMelody[0].noteNames.size() == 1) {
        
        float firstFreq = NoteParser::parseNote(trackMelody[0].noteNames[0]);
        if (firstFreq < 0) {
            cout << "  Error: Invalid note '" << trackMelody[0].noteNames[0] << "'" << endl;
            delete wave;
            return nullptr;
        }
        trackFullMelody = wave->generateSamples(firstFreq, sampleRate, trackMelody[0].duration);
        
        float* samples = trackFullMelody->getsamples();
        float finalVolume = trackInfo.volume * trackMelody[0].volume;
        for (int i = 0; i < trackFullMelody->getLength(); i++) {
            samples[i] *= finalVolume;
        }
        cout << "  Generated note 1 of " << trackMelody.size() << " (" << trackMelody[0].noteNames[0] << " = " << firstFreq << " Hz)" << endl;
    } else {
        
        float firstFreq = NoteParser::parseNote(trackMelody[0].noteNames[0]);
        if (firstFreq < 0) {
            cout << "  Error: Invalid note '" << trackMelody[0].noteNames[0] << "'" << endl;
            delete wave;
            return nullptr;
        }
        trackFullMelody = wave->generateSamples(firstFreq, sampleRate, trackMelody[0].duration);
        
        float* samples = trackFullMelody->getsamples();
        float finalVolume = trackInfo.volume * trackMelody[0].volume;
        for (int i = 0; i < trackFullMelody->getLength(); i++) {
            samples[i] *= (finalVolume / trackMelody[0].noteNames.size());  
        }
        
        
        for (size_t j = 1; j < trackMelody[0].noteNames.size(); j++) {
            float frequency = NoteParser::parseNote(trackMelody[0].noteNames[j]);
            if (frequency < 0) {
                cout << "  Error: Invalid note '" << trackMelody[0].noteNames[j] << "'" << endl;
                delete wave;
                delete trackFullMelody;
                return nullptr;
            }
            
            SoundSamples* note = wave->generateSamples(frequency, sampleRate, trackMelody[0].duration);
            
            float* noteSamples = note->getsamples();
            float noteVolume = finalVolume / trackMelody[0].noteNames.size();
            for (int i = 0; i < note->getLength(); i++) {
                (*trackFullMelody)[i] += (noteSamples[i] * noteVolume);
            }
            
            delete note;
        }
        
        cout << "  Generated chord 1 of " << trackMelody.size() << " (";
        for (size_t j = 0; j < trackMelody[0].noteNames.size(); j++) {
            if (j > 0) cout << ", ";
            float freq = NoteParser::parseNote(trackMelody[0].noteNames[j]);
            cout << trackMelody[0].noteNames[j] << " = " << freq << " Hz";
        }
        cout << ")" << endl;
    }

    
    for (size_t i = 1; i < trackMelody.size(); i++) {
        SoundSamples* noteSegment = nullptr;
        
        if (trackMelody[i].noteNames.empty()) {
            
            noteSegment = new SoundSamples(static_cast<int>(sampleRate * trackMelody[i].duration), sampleRate);
            cout << "  Generated rest " << (i + 1) << " of " << trackMelody.size() << " (duration: " << trackMelody[i].duration << "s)" << endl;
        } else if (trackMelody[i].noteNames.size() == 1) {
            
            float frequency = NoteParser::parseNote(trackMelody[i].noteNames[0]);
            if (frequency < 0) {
                cout << "  Error: Invalid note '" << trackMelody[i].noteNames[0] << "'" << endl;
                delete wave;
                delete trackFullMelody;
                return nullptr;
            }
            
            noteSegment = wave->generateSamples(frequency, sampleRate, trackMelody[i].duration);
            
            float* samples = noteSegment->getsamples();
            float finalVolume = trackInfo.volume * trackMelody[i].volume;
            for (int j = 0; j < noteSegment->getLength(); j++) {
                samples[j] *= finalVolume;
            }
            
            cout << "  Generated note " << (i + 1) << " of " << trackMelody.size() << " (" << trackMelody[i].noteNames[0] << " = " << frequency << " Hz)" << endl;
        } else {
            
            float firstFreq = NoteParser::parseNote(trackMelody[i].noteNames[0]);
            if (firstFreq < 0) {
                cout << "  Error: Invalid note '" << trackMelody[i].noteNames[0] << "'" << endl;
                delete wave;
                delete trackFullMelody;
                return nullptr;
            }
            
            noteSegment = wave->generateSamples(firstFreq, sampleRate, trackMelody[i].duration);
            
            float* samples = noteSegment->getsamples();
            float finalVolume = trackInfo.volume * trackMelody[i].volume;
            for (int j = 0; j < noteSegment->getLength(); j++) {
                samples[j] *= (finalVolume / trackMelody[i].noteNames.size());  
            }
            
            
            for (size_t k = 1; k < trackMelody[i].noteNames.size(); k++) {
                float frequency = NoteParser::parseNote(trackMelody[i].noteNames[k]);
                if (frequency < 0) {
                    cout << "  Error: Invalid note '" << trackMelody[i].noteNames[k] << "'" << endl;
                    delete wave;
                    delete trackFullMelody;
                    delete noteSegment;
                    return nullptr;
                }
                
                SoundSamples* note = wave->generateSamples(frequency, sampleRate, trackMelody[i].duration);
                
                float* noteSamples = note->getsamples();
                float noteVolume = finalVolume / trackMelody[i].noteNames.size();
                for (int j = 0; j < note->getLength(); j++) {
                    (*noteSegment)[j] += (noteSamples[j] * noteVolume);
                }
                
                delete note;
            }
            
            cout << "  Generated chord " << (i + 1) << " of " << trackMelody.size() << " (";
            for (size_t k = 0; k < trackMelody[i].noteNames.size(); k++) {
                if (k > 0) cout << ", ";
                float freq = NoteParser::parseNote(trackMelody[i].noteNames[k]);
                cout << trackMelody[i].noteNames[k] << " = " << freq << " Hz";
            }
            cout << ")" << endl;
        }

        
        SoundSamples combined = *trackFullMelody + *noteSegment;
        delete trackFullMelody;
        delete noteSegment;
        trackFullMelody = new SoundSamples(combined);

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

    vector<Note> melody = readNotesFromFile(inputFile);

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

        
        Track trackInfo;
        if (tracks.find(trackId) != tracks.end()) {
            trackInfo = tracks[trackId];
        } else {
            
            trackInfo = {trackId, waveType, 1.0f, "Default Track " + to_string(trackId)};
        }

        
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
            float mixedSample = fullData[i] + trackData[i];
            
            if (mixedSample > 1.0f) mixedSample = 1.0f;
            if (mixedSample < -1.0f) mixedSample = -1.0f;
            fullData[i] = mixedSample;
        }
        
        delete trackSamples;
    }

    cout << "================================================" << endl;
    cout << "Total duration: " << (fullMelody->getLength() / sampleRate) << " seconds" << endl;
    cout << "Total samples: " << fullMelody->getLength() << endl;

    
    if (outputWav) {
        bool success = WavExporter::exportToWav(fullMelody, outputFile, 16);
        if (success) {
            cout << "✓ WAV file generated: " << outputFile << endl;
            cout << "  Format: 16-bit PCM WAV, Mono, " << sampleRate << " Hz" << endl;
        } else {
            cout << "Error: Failed to generate WAV file" << endl;
            delete fullMelody;
            return 1;
        }
    } else {
        SoundIO::OutputSound(fullMelody, outputFile);
        cout << "✓ Raw audio file generated: " << outputFile << endl;
    }

    
    delete fullMelody;

    return 0;
}