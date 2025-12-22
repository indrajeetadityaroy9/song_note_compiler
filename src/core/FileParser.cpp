#include "core/FileParser.h"
#include "NoteParser.h"
#include "RhythmParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool FileParser::isChord(const string& noteStr) {
    return noteStr.find(',') != string::npos;
}

vector<string> FileParser::parseChord(const string& chordStr) {
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

vector<Note> FileParser::readNotesFromFile(const string& filename, SongContext& ctx) {
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
                        cerr << "Warning: Invalid wave type " << waveType
                             << " on line " << lineNumber << ", skipping" << endl;
                        continue;
                    }
                    ctx.defineTrack(trackId, waveType, trackVolume);
                    cout << "Defined track " << trackId << " (wave type: " << waveType
                         << ", volume: " << trackVolume << ")" << endl;
                } else {
                    cerr << "Warning: Invalid TRACK definition on line "
                         << lineNumber << ", skipping" << endl;
                }
                continue;
            }

            if (command == ">") {
                int trackId;
                if (iss >> trackId) {
                    if (ctx.switchTrack(trackId)) {
                        cout << "Switched to track " << trackId << endl;
                    } else {
                        cerr << "Warning: Undefined track " << trackId
                             << " on line " << lineNumber << ", using current track" << endl;
                    }
                } else {
                    cerr << "Warning: Invalid track switch on line "
                         << lineNumber << ", skipping" << endl;
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
                    cerr << "Warning: Invalid tempo value on line "
                         << lineNumber << ", skipping" << endl;
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
                cerr << "Warning: Could not parse duration on line "
                     << lineNumber << ": '" << line << "', skipping" << endl;
                continue;
            }

            iss >> volume;

            if (noteStr == "R" || noteStr == "r") {
                float duration = RhythmParser::parseRhythm(durationStr, ctx.tempo, ctx.timeSignature);
                if (duration < 0) {
                    cerr << "Warning: Invalid duration '" << durationStr
                         << "' on line " << lineNumber << ", skipping" << endl;
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
                        cerr << "Warning: Invalid note '" << note
                             << "' in chord on line " << lineNumber << ", skipping line" << endl;
                        validChord = false;
                        break;
                    }
                }

                if (!validChord) {
                    continue;
                }
            } else {
                if (!NoteParser::isValidNote(noteStr)) {
                    cerr << "Warning: Invalid note '" << noteStr
                         << "' on line " << lineNumber << ", skipping" << endl;
                    continue;
                }
                noteNames.push_back(noteStr);
            }

            float duration = RhythmParser::parseRhythm(durationStr, ctx.tempo, ctx.timeSignature);
            if (duration < 0) {
                cerr << "Warning: Invalid duration '" << durationStr
                     << "' on line " << lineNumber << ", skipping" << endl;
                continue;
            }

            volume = std::clamp(volume, 0.0f, 1.0f);

            notes.push_back({noteNames, durationStr, duration, volume, ctx.currentTrack});
        } else {
            cerr << "Warning: Could not parse line " << lineNumber
                 << ": '" << line << "', skipping" << endl;
        }
    }

    file.close();
    return notes;
}

bool FileParser::saveNotesToFile(const vector<Note>& notes, const SongContext& ctx,
                                  const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "' for writing" << endl;
        return false;
    }

    // Write header
    file << "# Song Note Compiler file" << endl;
    file << "TEMPO " << ctx.tempo << endl;
    file << "TIME " << ctx.timeSignature << endl;
    file << endl;

    // Write track definitions
    for (const auto& pair : ctx.tracks) {
        const Track& track = pair.second;
        file << "TRACK " << track.id << " " << track.waveType
             << " " << track.volume << endl;
    }
    file << endl;

    // Write notes grouped by track
    int lastTrack = -1;
    for (const Note& note : notes) {
        if (note.trackId != lastTrack) {
            file << "> " << note.trackId << endl;
            lastTrack = note.trackId;
        }

        if (note.noteNames.empty()) {
            file << "R";
        } else if (note.noteNames.size() == 1) {
            file << note.noteNames[0];
        } else {
            for (size_t i = 0; i < note.noteNames.size(); i++) {
                if (i > 0) file << ",";
                file << note.noteNames[i];
            }
        }

        file << " " << note.durationStr;
        if (note.volume != 1.0f) {
            file << " " << note.volume;
        }
        file << endl;
    }

    file.close();
    return true;
}
