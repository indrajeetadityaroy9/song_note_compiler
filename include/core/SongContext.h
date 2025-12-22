#ifndef SONG_NOTE_COMPILER_SONGCONTEXT_H
#define SONG_NOTE_COMPILER_SONGCONTEXT_H

#include <string>
#include <map>
#include <algorithm>
#include "Track.h"

class SongContext {
public:
    float tempo = 120.0f;
    std::string timeSignature = "4/4";
    std::map<int, Track> tracks;
    int currentTrack = 1;

    void setTempo(float t) { tempo = t; }
    float getTempo() const { return tempo; }
    void setTimeSignature(const std::string& ts) { timeSignature = ts; }
    std::string getTimeSignature() const { return timeSignature; }
    std::map<int, Track>& getTracks() { return tracks; }
    const std::map<int, Track>& getTracks() const { return tracks; }

    void defineTrack(int id, int waveType, float vol) {
        vol = std::clamp(vol, 0.0f, 1.0f);
        tracks[id] = {id, waveType, vol, "Track " + std::to_string(id)};
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
        return {trackId, defaultWaveType, 1.0f, "Default Track " + std::to_string(trackId)};
    }
};

#endif
