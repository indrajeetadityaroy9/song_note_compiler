#ifndef APPLICATION_H
#define APPLICATION_H

#include "gui/EditorState.h"
#include "gui/AudioPlayer.h"
#include <string>

struct GLFWwindow;

class Application {
public:
    Application();
    ~Application();

    bool initialize();
    void run();
    void shutdown();

private:
    void renderMainWindow();
    void renderMenuBar();
    void renderTransportPanel();
    void renderTrackPanel();
    void renderNoteEditorPanel();
    void renderWaveformPanel();
    void renderStatusBar();
    void renderTemplatesPanel();

    void generateAudio();
    void exportWav(const std::string& path);
    void loadTemplate(int templateIndex);

    GLFWwindow* m_window = nullptr;
    EditorState m_state;
    AudioPlayer m_audioPlayer;

    char m_exportPathBuffer[256] = "output.wav";

    // Note editor state
    char m_noteNameBuffer[64] = "C4";
    char m_durationBuffer[16] = "q";
    float m_noteVolume = 1.0f;
};

#endif
