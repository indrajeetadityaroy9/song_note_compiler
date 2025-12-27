#include "core/MusicXMLParser.h"
#include "core/MusicXMLTypes.h"
#include "NoteParser.h"
#include "RhythmParser.h"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace pugi;

// =============================================================================
// IMPORT IMPLEMENTATION
// =============================================================================

vector<Note> MusicXMLParser::readFromFile(const string& filename, SongContext& ctx) {
    vector<Note> allNotes;

    xml_document doc;
    xml_parse_result result = doc.load_file(filename.c_str());

    if (!result) {
        cerr << "Error: Could not parse MusicXML file '" << filename << "': "
             << result.description() << endl;
        return allNotes;
    }

    // Find the score-partwise root element
    xml_node scorePW = doc.child("score-partwise");
    if (!scorePW) {
        cerr << "Error: Not a valid MusicXML score-partwise document" << endl;
        return allNotes;
    }

    // Parse part-list to create tracks
    xml_node partList = scorePW.child("part-list");
    if (partList) {
        parsePartList(partList, ctx);
    }

    // Parse each part
    int trackId = 1;

    for (xml_node part = scorePW.child("part"); part; part = part.next_sibling("part")) {
        string partId = part.attribute("id").as_string();
        int divisions = DEFAULT_DIVISIONS;

        // Find matching track or create one
        bool found = false;
        for (auto& pair : ctx.getTracks()) {
            if (pair.second.partId == partId) {
                trackId = pair.first;
                found = true;
                break;
            }
        }
        if (!found) {
            ctx.defineTrack(trackId, 1, 1.0f);
            ctx.getTracks()[trackId].partId = partId;
        }

        vector<Note> partNotes = parsePart(part, trackId, ctx, divisions);
        allNotes.insert(allNotes.end(), partNotes.begin(), partNotes.end());

        trackId++;
    }

    cout << "Loaded " << allNotes.size() << " notes from MusicXML file" << endl;
    return allNotes;
}

void MusicXMLParser::parsePartList(const xml_node& partList, SongContext& ctx) {
    int trackId = 1;

    for (xml_node scorePart = partList.child("score-part"); scorePart;
         scorePart = scorePart.next_sibling("score-part")) {

        string partId = scorePart.attribute("id").as_string();
        string partName = scorePart.child_value("part-name");

        if (partName.empty()) {
            partName = "Track " + to_string(trackId);
        }

        ctx.defineTrack(trackId, 1, 1.0f);  // Default: sine wave, full volume
        ctx.getTracks()[trackId].name = partName;
        ctx.getTracks()[trackId].partId = partId;

        trackId++;
    }
}

vector<Note> MusicXMLParser::parsePart(const xml_node& part, int trackId,
                                        SongContext& ctx, int& divisions) {
    vector<Note> notes;
    float currentTempo = ctx.getTempo();

    for (xml_node measure = part.child("measure"); measure;
         measure = measure.next_sibling("measure")) {

        int measureNum = measure.attribute("number").as_int();

        // Process elements in order
        for (xml_node child = measure.first_child(); child; child = child.next_sibling()) {
            string name = child.name();

            if (name == "attributes") {
                parseAttributes(child, divisions, ctx);
            }
            else if (name == "direction") {
                float newTempo = parseDirection(child, currentTempo);
                if (newTempo > 0) {
                    currentTempo = newTempo;
                    ctx.setTempo(currentTempo);
                }
            }
            else if (name == "note") {
                // Check if this is a chord continuation
                bool isChordContinuation = child.child("chord");

                Note note = parseNote(child, trackId, divisions, currentTempo);
                note.measureNumber = measureNum;

                if (isChordContinuation && !notes.empty()) {
                    // Add to previous note as chord
                    Note& prevNote = notes.back();
                    for (const string& nn : note.noteNames) {
                        if (nn != "R" && !nn.empty()) {
                            prevNote.noteNames.push_back(nn);
                        }
                    }
                } else {
                    notes.push_back(note);
                }
            }
        }
    }

    return notes;
}

Note MusicXMLParser::parseNote(const xml_node& noteNode, int trackId,
                                int divisions, float tempo) {
    Note note;
    note.trackId = trackId;
    note.volume = 1.0f;
    note.divisions = divisions;

    // Check for rest
    if (noteNode.child("rest")) {
        note.noteNames.clear();  // Empty noteNames = rest

        int dur = noteNode.child("duration").text().as_int();
        note.mxmlDuration = dur;

        float beats = RhythmParser::musicXMLDurationToBeats(dur, divisions);
        note.durationStr = RhythmParser::beatsToTypeString(beats);
        note.duration = RhythmParser::beatsToSeconds(beats, tempo);

        return note;
    }

    // Parse pitch
    xml_node pitch = noteNode.child("pitch");
    if (pitch) {
        MusicXMLPitch mxmlPitch;
        const char* stepStr = pitch.child_value("step");
        mxmlPitch.step = (stepStr && stepStr[0]) ? stepStr[0] : 'C';
        mxmlPitch.alter = pitch.child("alter").text().as_int(0);
        mxmlPitch.octave = pitch.child("octave").text().as_int(4);

        string noteName = NoteParser::pitchToNoteName(mxmlPitch);
        note.noteNames.push_back(noteName);
    }

    // Parse duration
    int dur = noteNode.child("duration").text().as_int();
    note.mxmlDuration = dur;

    // Parse type for more accurate duration string
    string typeStr = noteNode.child_value("type");
    bool hasDot = noteNode.child("dot");

    if (!typeStr.empty()) {
        float beats = RhythmParser::typeStringToBeats(typeStr);
        if (hasDot) beats *= 1.5f;
        note.durationStr = RhythmParser::beatsToTypeString(beats);
        if (hasDot) note.durationStr += ".";
    } else if (dur > 0 && divisions > 0) {
        float beats = RhythmParser::musicXMLDurationToBeats(dur, divisions);
        note.durationStr = RhythmParser::beatsToTypeString(beats);
    } else {
        note.durationStr = "q";  // Default to quarter note
    }

    float beats = RhythmParser::musicXMLDurationToBeats(dur, divisions);
    note.duration = RhythmParser::beatsToSeconds(beats, tempo);

    return note;
}

void MusicXMLParser::parseAttributes(const xml_node& attributes,
                                      int& divisions, SongContext& ctx) {
    // Parse divisions
    xml_node divNode = attributes.child("divisions");
    if (divNode) {
        divisions = divNode.text().as_int(DEFAULT_DIVISIONS);
    }

    // Parse time signature
    xml_node time = attributes.child("time");
    if (time) {
        int beats = time.child("beats").text().as_int(4);
        int beatType = time.child("beat-type").text().as_int(4);
        ctx.setTimeSignature(to_string(beats) + "/" + to_string(beatType));
    }
}

float MusicXMLParser::parseDirection(const xml_node& direction, float currentTempo) {
    xml_node sound = direction.child("sound");
    if (sound) {
        float tempo = sound.attribute("tempo").as_float(0);
        if (tempo > 0) {
            return tempo;
        }
    }
    return 0;  // No tempo change
}

// =============================================================================
// EXPORT IMPLEMENTATION
// =============================================================================

bool MusicXMLParser::writeToFile(const vector<Note>& notes,
                                  const SongContext& ctx,
                                  const string& filename) {
    xml_document doc;

    // XML declaration
    xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    // DOCTYPE
    xml_node doctype = doc.append_child(pugi::node_doctype);
    doctype.set_value("score-partwise PUBLIC \"-//Recordare//DTD MusicXML 4.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\"");

    // Root element
    xml_node scorePW = doc.append_child("score-partwise");
    scorePW.append_attribute("version") = "4.0";

    // Write part-list
    writePartList(scorePW, ctx);

    // Group notes by track
    map<int, vector<Note>> trackNotes;
    for (const Note& note : notes) {
        trackNotes[note.trackId].push_back(note);
    }

    // Write each part
    int divisions = DEFAULT_DIVISIONS;
    for (const auto& pair : ctx.getTracks()) {
        int trackId = pair.first;
        auto it = trackNotes.find(trackId);
        if (it != trackNotes.end()) {
            writePart(scorePW, trackId, it->second, ctx, divisions);
        } else {
            // Write empty part
            writePart(scorePW, trackId, {}, ctx, divisions);
        }
    }

    // If no tracks defined, write at least one part
    if (ctx.getTracks().empty()) {
        SongContext& mutableCtx = const_cast<SongContext&>(ctx);
        mutableCtx.defineTrack(1, 1, 1.0f);
        writePart(scorePW, 1, notes, ctx, divisions);
    }

    // Save to file
    bool success = doc.save_file(filename.c_str(), "  ");

    if (success) {
        cout << "Saved MusicXML to " << filename << endl;
    } else {
        cerr << "Error: Could not save MusicXML file '" << filename << "'" << endl;
    }

    return success;
}

void MusicXMLParser::writePartList(xml_node& root, const SongContext& ctx) {
    xml_node partList = root.append_child("part-list");

    for (const auto& pair : ctx.getTracks()) {
        const Track& track = pair.second;

        xml_node scorePart = partList.append_child("score-part");
        string partId = track.partId.empty() ? ("P" + to_string(track.id)) : track.partId;
        scorePart.append_attribute("id") = partId.c_str();

        xml_node partName = scorePart.append_child("part-name");
        partName.text().set(track.name.c_str());
    }
}

void MusicXMLParser::writePart(xml_node& root, int trackId,
                                const vector<Note>& trackNotes,
                                const SongContext& ctx, int divisions) {
    // Get track info
    Track trackInfo = ctx.getTrackInfo(trackId, 1);
    string partId = trackInfo.partId.empty() ? ("P" + to_string(trackId)) : trackInfo.partId;

    xml_node part = root.append_child("part");
    part.append_attribute("id") = partId.c_str();

    // Organize notes into measures
    vector<vector<Note>> measures = organizeIntoMeasures(trackNotes, ctx, divisions);

    // Write each measure
    for (size_t i = 0; i < measures.size(); i++) {
        writeMeasure(part, i + 1, measures[i], ctx, divisions, i == 0);
    }

    // If no notes, write one empty measure
    if (measures.empty()) {
        writeMeasure(part, 1, {}, ctx, divisions, true);
    }
}

void MusicXMLParser::writeMeasure(xml_node& part, int measureNum,
                                   const vector<Note>& measureNotes,
                                   const SongContext& ctx, int divisions,
                                   bool isFirstMeasure) {
    xml_node measure = part.append_child("measure");
    measure.append_attribute("number") = measureNum;

    // Write attributes in first measure
    if (isFirstMeasure) {
        xml_node attributes = measure.append_child("attributes");

        attributes.append_child("divisions").text().set(divisions);

        // Time signature
        string timeSig = ctx.getTimeSignature();
        size_t slashPos = timeSig.find('/');
        int beats = 4, beatType = 4;
        if (slashPos != string::npos) {
            beats = stoi(timeSig.substr(0, slashPos));
            beatType = stoi(timeSig.substr(slashPos + 1));
        }

        xml_node time = attributes.append_child("time");
        time.append_child("beats").text().set(beats);
        time.append_child("beat-type").text().set(beatType);

        // Clef (default to treble)
        xml_node clef = attributes.append_child("clef");
        clef.append_child("sign").text().set("G");
        clef.append_child("line").text().set(2);
    }

    // Write tempo direction in first measure
    if (isFirstMeasure) {
        xml_node direction = measure.append_child("direction");
        direction.append_attribute("placement") = "above";

        xml_node directionType = direction.append_child("direction-type");
        xml_node metronome = directionType.append_child("metronome");
        metronome.append_child("beat-unit").text().set("quarter");
        metronome.append_child("per-minute").text().set(static_cast<int>(ctx.getTempo()));

        xml_node sound = direction.append_child("sound");
        sound.append_attribute("tempo") = ctx.getTempo();
    }

    // Write notes
    for (size_t i = 0; i < measureNotes.size(); i++) {
        const Note& note = measureNotes[i];

        if (note.noteNames.empty() ||
            (note.noteNames.size() == 1 && (note.noteNames[0] == "R" || note.noteNames[0] == "r"))) {
            writeRest(measure, note, divisions);
        } else if (note.noteNames.size() == 1) {
            writeNote(measure, note, divisions, false);
        } else {
            // Chord: write first note without <chord>, rest with <chord>
            Note singleNote = note;
            for (size_t j = 0; j < note.noteNames.size(); j++) {
                singleNote.noteNames = {note.noteNames[j]};
                writeNote(measure, singleNote, divisions, j > 0);
            }
        }
    }
}

void MusicXMLParser::writeNote(xml_node& measure, const Note& note,
                                int divisions, bool isChordNote) {
    xml_node noteNode = measure.append_child("note");

    // Chord marker (for all notes after the first in a chord)
    if (isChordNote) {
        noteNode.append_child("chord");
    }

    // Pitch
    if (!note.noteNames.empty() && note.noteNames[0] != "R" && note.noteNames[0] != "r") {
        MusicXMLPitch pitch = NoteParser::noteNameToPitch(note.noteNames[0]);

        xml_node pitchNode = noteNode.append_child("pitch");
        char stepStr[2] = {pitch.step, '\0'};
        pitchNode.append_child("step").text().set(stepStr);
        if (pitch.alter != 0) {
            pitchNode.append_child("alter").text().set(pitch.alter);
        }
        pitchNode.append_child("octave").text().set(pitch.octave);
    }

    // Duration
    MusicXMLDuration mxmlDur = RhythmParser::durationStrToMusicXML(note.durationStr, divisions);
    noteNode.append_child("duration").text().set(mxmlDur.divisions);

    // Type
    noteNode.append_child("type").text().set(mxmlDur.type.c_str());

    // Dots
    for (int i = 0; i < mxmlDur.dots; i++) {
        noteNode.append_child("dot");
    }
}

void MusicXMLParser::writeRest(xml_node& measure, const Note& note, int divisions) {
    xml_node noteNode = measure.append_child("note");
    noteNode.append_child("rest");

    MusicXMLDuration mxmlDur = RhythmParser::durationStrToMusicXML(note.durationStr, divisions);
    noteNode.append_child("duration").text().set(mxmlDur.divisions);
    noteNode.append_child("type").text().set(mxmlDur.type.c_str());

    for (int i = 0; i < mxmlDur.dots; i++) {
        noteNode.append_child("dot");
    }
}

vector<vector<Note>> MusicXMLParser::organizeIntoMeasures(
    const vector<Note>& notes,
    const SongContext& ctx,
    int divisions) {

    vector<vector<Note>> measures;

    // Parse time signature to get beats per measure
    float beatsPerMeasure = RhythmParser::parseTimeSignature(ctx.getTimeSignature());

    vector<Note> currentMeasure;
    float currentBeatCount = 0;

    for (const Note& note : notes) {
        // Calculate note duration in beats
        float beats = RhythmParser::getNoteBeatValue(note.durationStr.empty() ? 'q' : note.durationStr[0]);
        if (beats < 0) beats = 1.0f;  // Default to quarter note

        // Apply modifiers
        for (size_t i = 1; i < note.durationStr.size(); i++) {
            if (note.durationStr[i] == '.') beats *= 1.5f;
            else if (note.durationStr[i] == 't') beats *= 2.0f/3.0f;
        }

        // Check if note fits in current measure
        if (currentBeatCount + beats > beatsPerMeasure + 0.001f) {
            // Start new measure
            if (!currentMeasure.empty()) {
                measures.push_back(currentMeasure);
            }
            currentMeasure.clear();
            currentBeatCount = 0;
        }

        currentMeasure.push_back(note);
        currentBeatCount += beats;

        // If measure is exactly full, start new one
        if (fabs(currentBeatCount - beatsPerMeasure) < 0.001f) {
            measures.push_back(currentMeasure);
            currentMeasure.clear();
            currentBeatCount = 0;
        }
    }

    // Add final partial measure
    if (!currentMeasure.empty()) {
        measures.push_back(currentMeasure);
    }

    return measures;
}

// =============================================================================
// UTILITY METHODS (for compatibility with FileParser)
// =============================================================================

bool MusicXMLParser::isChord(const string& noteStr) {
    return noteStr.find(',') != string::npos;
}

vector<string> MusicXMLParser::parseChord(const string& chordStr) {
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
