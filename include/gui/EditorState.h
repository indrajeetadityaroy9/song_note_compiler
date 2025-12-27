#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "core/Note.h"
#include "core/Track.h"
#include "core/SongContext.h"
#include "SoundSamples.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

struct EditorState {
    std::vector<Note> notes;
    SongContext songContext;

    int selectedNoteIndex = -1;
    int selectedTrackId = 1;
    int defaultWaveType = 1;  // 1=Sine, 2=Square, 3=Triangle, 4=Sawtooth

    float sampleRate = 44100.0f;

    bool isPlaying = false;
    bool isPaused = false;
    size_t playbackPosition = 0;

    std::string currentFilePath;
    bool hasUnsavedChanges = false;
    std::string statusMessage;

    // Inline note editing state (moved from static variables)
    char editNoteBuffer[64] = "";
    char editDurBuffer[16] = "";
    float editVolume = 1.0f;

    // Per-track generated audio
    std::map<int, std::unique_ptr<SoundSamples>> trackAudio;
    // Mixed audio (all tracks combined)
    std::unique_ptr<SoundSamples> generatedAudio;
    bool audioNeedsRegeneration = true;

    void markDirty() {
        hasUnsavedChanges = true;
        audioNeedsRegeneration = true;
    }

    // Get audio for current selected track (or mixed if not available)
    SoundSamples* getSelectedTrackAudio() {
        auto it = trackAudio.find(selectedTrackId);
        if (it != trackAudio.end() && it->second) {
            return it->second.get();
        }
        return generatedAudio.get();
    }

    void setStatus(const std::string& msg) {
        statusMessage = msg;
    }
};

#endif
