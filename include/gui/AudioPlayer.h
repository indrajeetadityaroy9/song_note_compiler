#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include "SoundSamples.h"
#include <atomic>
#include <mutex>

struct ma_device;

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    bool initialize(float sampleRate);
    void shutdown();

    void play(SoundSamples* samples);
    void stop();
    void pause();
    void resume();

    bool isPlaying() const { return m_isPlaying.load(); }
    bool isPaused() const { return m_isPaused.load(); }

    size_t getPlaybackPosition() const { return m_playbackPosition.load(); }
    void setPlaybackPosition(size_t pos);

    float getSampleRate() const { return m_sampleRate; }

    void processAudio(float* output, unsigned int frameCount);

private:
    void* m_device = nullptr;  // ma_device*
    float m_sampleRate = 44100.0f;

    SoundSamples* m_currentSamples = nullptr;
    std::mutex m_samplesMutex;

    std::atomic<bool> m_isPlaying{false};
    std::atomic<bool> m_isPaused{false};
    std::atomic<size_t> m_playbackPosition{0};
    std::atomic<bool> m_initialized{false};
};

#endif
