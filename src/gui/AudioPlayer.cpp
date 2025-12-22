#include "gui/AudioPlayer.h"
#include "miniaudio.h"
#include <cstring>

static void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput,
                          ma_uint32 frameCount) {
    (void)pInput;
    AudioPlayer* player = static_cast<AudioPlayer*>(pDevice->pUserData);
    player->processAudio(static_cast<float*>(pOutput), frameCount);
}

AudioPlayer::AudioPlayer() {}

AudioPlayer::~AudioPlayer() {
    shutdown();
}

bool AudioPlayer::initialize(float sampleRate) {
    if (m_initialized.load()) {
        return true;
    }

    m_sampleRate = sampleRate;

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = 1;
    config.sampleRate = static_cast<ma_uint32>(sampleRate);
    config.dataCallback = audioCallback;
    config.pUserData = this;

    m_device = new ma_device;
    if (ma_device_init(nullptr, &config, static_cast<ma_device*>(m_device)) != MA_SUCCESS) {
        delete static_cast<ma_device*>(m_device);
        m_device = nullptr;
        return false;
    }

    m_initialized.store(true);
    return true;
}

void AudioPlayer::shutdown() {
    if (!m_initialized.load()) {
        return;
    }

    stop();

    if (m_device) {
        ma_device_uninit(static_cast<ma_device*>(m_device));
        delete static_cast<ma_device*>(m_device);
        m_device = nullptr;
    }

    m_initialized.store(false);
}

void AudioPlayer::play(SoundSamples* samples) {
    if (!m_initialized.load() || !samples) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_samplesMutex);
        m_currentSamples = samples;
    }

    m_playbackPosition.store(0);
    m_isPaused.store(false);
    m_isPlaying.store(true);

    ma_device_start(static_cast<ma_device*>(m_device));
}

void AudioPlayer::stop() {
    if (!m_initialized.load()) {
        return;
    }

    m_isPlaying.store(false);
    m_isPaused.store(false);
    m_playbackPosition.store(0);

    ma_device_stop(static_cast<ma_device*>(m_device));

    {
        std::lock_guard<std::mutex> lock(m_samplesMutex);
        m_currentSamples = nullptr;
    }
}

void AudioPlayer::pause() {
    if (!m_isPlaying.load()) {
        return;
    }

    m_isPaused.store(true);
    ma_device_stop(static_cast<ma_device*>(m_device));
}

void AudioPlayer::resume() {
    if (!m_isPaused.load()) {
        return;
    }

    m_isPaused.store(false);
    ma_device_start(static_cast<ma_device*>(m_device));
}

void AudioPlayer::setPlaybackPosition(size_t pos) {
    m_playbackPosition.store(pos);
}

void AudioPlayer::processAudio(float* output, unsigned int frameCount) {
    if (!m_isPlaying.load() || m_isPaused.load()) {
        std::memset(output, 0, frameCount * sizeof(float));
        return;
    }

    std::lock_guard<std::mutex> lock(m_samplesMutex);

    if (!m_currentSamples) {
        std::memset(output, 0, frameCount * sizeof(float));
        return;
    }

    size_t pos = m_playbackPosition.load();
    size_t sampleCount = static_cast<size_t>(m_currentSamples->getLength());
    float* sampleData = m_currentSamples->getsamples();

    for (unsigned int i = 0; i < frameCount; ++i) {
        if (pos < sampleCount) {
            output[i] = sampleData[pos];
            ++pos;
        } else {
            output[i] = 0.0f;
        }
    }

    m_playbackPosition.store(pos);

    if (pos >= sampleCount) {
        m_isPlaying.store(false);
    }
}
