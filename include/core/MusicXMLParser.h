#ifndef SONG_NOTE_COMPILER_MUSICXMLPARSER_H
#define SONG_NOTE_COMPILER_MUSICXMLPARSER_H

#include <string>
#include <vector>
#include <map>
#include "Note.h"
#include "SongContext.h"

// Forward declaration for pugixml
namespace pugi {
    class xml_node;
    class xml_document;
}

class MusicXMLParser {
public:
    // Main API - drop-in replacement for FileParser
    static std::vector<Note> readFromFile(
        const std::string& filename,
        SongContext& ctx
    );

    static bool writeToFile(
        const std::vector<Note>& notes,
        const SongContext& ctx,
        const std::string& filename
    );

    // Utility methods for chord detection (kept for compatibility)
    static bool isChord(const std::string& noteStr);
    static std::vector<std::string> parseChord(const std::string& chordStr);

private:
    // Import helpers
    static void parsePartList(const pugi::xml_node& partList, SongContext& ctx);
    static std::vector<Note> parsePart(const pugi::xml_node& part, int trackId,
                                       SongContext& ctx, int& divisions);
    static Note parseNote(const pugi::xml_node& noteNode, int trackId,
                          int divisions, float tempo);
    static void parseAttributes(const pugi::xml_node& attributes,
                                int& divisions, SongContext& ctx);
    static float parseDirection(const pugi::xml_node& direction, float currentTempo);

    // Export helpers
    static void writePartList(pugi::xml_node& root, const SongContext& ctx);
    static void writePart(pugi::xml_node& root, int trackId,
                          const std::vector<Note>& trackNotes,
                          const SongContext& ctx, int divisions);
    static void writeMeasure(pugi::xml_node& part, int measureNum,
                             const std::vector<Note>& measureNotes,
                             const SongContext& ctx, int divisions,
                             bool isFirstMeasure);
    static void writeNote(pugi::xml_node& measure, const Note& note,
                          int divisions, bool isChordNote);
    static void writeRest(pugi::xml_node& measure, const Note& note, int divisions);

    // Measure organization
    static std::vector<std::vector<Note>> organizeIntoMeasures(
        const std::vector<Note>& notes,
        const SongContext& ctx,
        int divisions
    );

    // Constants
    static const int DEFAULT_DIVISIONS = 24;  // Divisions per quarter note
};

#endif // SONG_NOTE_COMPILER_MUSICXMLPARSER_H
