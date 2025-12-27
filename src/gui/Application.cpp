#include "gui/Application.h"
#include "core/SongGenerator.h"
#include "core/MusicXMLParser.h"
#include "WavExporter.h"
#include "RhythmParser.h"
#include "NoteParser.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <algorithm>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Application::Application() {}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return false;
    }

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(1400, 900, "Song Note Compiler", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!m_audioPlayer.initialize(m_state.sampleRate)) {
        m_state.setStatus("Warning: Audio device initialization failed");
    }

    m_state.songContext.setTempo(120.0f);
    m_state.songContext.defineTrack(1, m_state.defaultWaveType, 1.0f);  // Ensure track 1 exists
    m_state.selectedTrackId = 1;
    m_state.setStatus("Ready");

    return true;
}

void Application::run() {
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderMainWindow();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);

        if (m_audioPlayer.isPlaying()) {
            m_state.playbackPosition = m_audioPlayer.getPlaybackPosition();
        }
    }
}

void Application::shutdown() {
    m_audioPlayer.shutdown();

    if (m_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void Application::renderMainWindow() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));

    ImGui::Begin("MainWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    renderMenuBar();
    renderTransportPanel();

    float panelWidth = ImGui::GetContentRegionAvail().x * 0.3f;

    ImGui::BeginChild("LeftPanel", ImVec2(panelWidth, 0), true);
    renderTrackPanel();
    renderTemplatesPanel();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    renderNoteEditorPanel();
    ImGui::Separator();
    renderWaveformPanel();
    ImGui::EndChild();

    renderStatusBar();

    ImGui::End();
}

void Application::renderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open MusicXML...", "Ctrl+O")) {
                openMusicXMLFile();
            }
            if (ImGui::MenuItem("Save MusicXML", "Ctrl+S")) {
                saveMusicXMLFile();
            }
            if (ImGui::MenuItem("Save MusicXML As...")) {
                saveMusicXMLFileAs();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Export WAV...")) {
                if (m_state.generatedAudio) {
                    exportWav(m_exportPathBuffer);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(m_window, true);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void Application::renderTransportPanel() {
    ImGui::BeginChild("TransportPanel", ImVec2(0, 120), true);

    ImGui::Text("Transport");
    ImGui::Separator();

    bool isPlaying = m_audioPlayer.isPlaying();
    bool isPaused = m_audioPlayer.isPaused();

    if (isPlaying && !isPaused) {
        if (ImGui::Button("Pause", ImVec2(80, 30))) {
            m_audioPlayer.pause();
            m_state.setStatus("Paused");
        }
    } else {
        if (ImGui::Button("Play", ImVec2(80, 30))) {
            if (isPaused) {
                m_audioPlayer.resume();
                m_state.setStatus("Playing Track " + std::to_string(m_state.selectedTrackId) + "...");
            } else {
                if (m_state.audioNeedsRegeneration || !m_state.generatedAudio) {
                    generateAudio();
                }
                // Play selected track audio
                SoundSamples* trackAudio = m_state.getSelectedTrackAudio();
                if (trackAudio) {
                    m_audioPlayer.play(trackAudio);
                    m_state.setStatus("Playing Track " + std::to_string(m_state.selectedTrackId) + "...");
                }
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Play All", ImVec2(80, 30))) {
        if (m_state.audioNeedsRegeneration || !m_state.generatedAudio) {
            generateAudio();
        }
        if (m_state.generatedAudio) {
            m_audioPlayer.stop();
            m_audioPlayer.play(m_state.generatedAudio.get());
            m_state.setStatus("Playing all tracks...");
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Generate", ImVec2(80, 30))) {
        generateAudio();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    float tempo = m_state.songContext.getTempo();
    if (ImGui::InputFloat("Tempo", &tempo, 5.0f, 20.0f, "%.0f BPM")) {
        if (tempo < 20.0f) tempo = 20.0f;
        if (tempo > 300.0f) tempo = 300.0f;
        m_state.songContext.setTempo(tempo);
        m_state.markDirty();
    }

    ImGui::Separator();

    ImGui::SetNextItemWidth(300);
    ImGui::InputText("Export Path", m_exportPathBuffer, sizeof(m_exportPathBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Export WAV")) {
        if (m_state.audioNeedsRegeneration || !m_state.generatedAudio) {
            generateAudio();
        }
        exportWav(m_exportPathBuffer);
    }

    ImGui::EndChild();
}

void Application::renderTrackPanel() {
    ImGui::Text("Tracks");
    ImGui::Separator();

    const char* waveTypes[] = { "Sine", "Square", "Triangle", "Sawtooth" };
    const char* timeSigs[] = { "4/4", "3/4", "2/4", "6/8" };

    // Time signature
    ImGui::Text("Time Signature:");
    ImGui::SetNextItemWidth(-1);
    std::string currentTimeSig = m_state.songContext.getTimeSignature();
    int timeSigIdx = 0;
    for (int i = 0; i < 4; i++) {
        if (currentTimeSig == timeSigs[i]) {
            timeSigIdx = i;
            break;
        }
    }
    if (ImGui::Combo("##TimeSig", &timeSigIdx, timeSigs, 4)) {
        m_state.songContext.setTimeSignature(timeSigs[timeSigIdx]);
        m_state.markDirty();
    }

    ImGui::Separator();

    // Track list with inline editing
    auto& tracks = m_state.songContext.getTracks();

    if (tracks.empty()) {
        ImGui::TextDisabled("No tracks defined");
    }

    for (auto& pair : tracks) {
        Track& track = pair.second;
        ImGui::PushID(track.id);

        bool selected = (m_state.selectedTrackId == track.id);

        // Track header (clickable)
        if (ImGui::Selectable(("Track " + std::to_string(track.id)).c_str(), selected)) {
            if (m_state.selectedTrackId != track.id) {
                m_state.selectedTrackId = track.id;
                m_state.selectedNoteIndex = -1;  // Deselect note when changing tracks
                m_audioPlayer.stop();  // Stop playback when changing tracks
                m_state.playbackPosition = 0;
            }
        }

        // Show editable properties for selected track
        if (selected) {
            ImGui::Indent();

            // Wave type selector
            ImGui::Text("Wave:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            int waveIdx = track.waveType - 1;
            if (ImGui::Combo("##TrackWave", &waveIdx, waveTypes, 4)) {
                track.waveType = waveIdx + 1;
                m_state.markDirty();
            }

            // Volume slider
            ImGui::Text("Volume:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            if (ImGui::SliderFloat("##TrackVol", &track.volume, 0.0f, 1.0f, "%.2f")) {
                m_state.markDirty();
            }

            // Delete track button (only if more than one track or no notes use it)
            bool canDelete = tracks.size() > 1;
            if (!canDelete) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Delete Track")) {
                // Find the first track that isn't being deleted for reassignment
                int reassignToTrack = -1;
                for (auto& p : tracks) {
                    if (p.first != track.id) {
                        reassignToTrack = p.first;
                        break;
                    }
                }
                // Reassign notes from deleted track
                for (auto& note : m_state.notes) {
                    if (note.trackId == track.id) {
                        note.trackId = reassignToTrack;
                    }
                }
                tracks.erase(track.id);
                m_state.selectedTrackId = tracks.begin()->first;
                m_state.markDirty();
                ImGui::Unindent();
                ImGui::PopID();
                break;  // Exit loop since we modified the map
            }
            if (!canDelete) {
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::TextDisabled("(need 2+ tracks)");
            }

            ImGui::Unindent();
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    // Default wave type for new tracks
    ImGui::Text("New Track Wave:");
    ImGui::SetNextItemWidth(-1);
    int defaultWaveIdx = m_state.defaultWaveType - 1;
    if (ImGui::Combo("##DefaultWave", &defaultWaveIdx, waveTypes, 4)) {
        m_state.defaultWaveType = defaultWaveIdx + 1;
    }

    if (ImGui::Button("Add Track", ImVec2(-1, 0))) {
        int newId = tracks.empty() ? 1 : tracks.rbegin()->first + 1;
        m_state.songContext.defineTrack(newId, m_state.defaultWaveType, 1.0f);
        m_state.selectedTrackId = newId;
        m_state.markDirty();
    }
}

void Application::renderNoteEditorPanel() {
    // Count notes for selected track
    size_t trackNoteCount = 0;
    for (const auto& note : m_state.notes) {
        if (note.trackId == m_state.selectedTrackId) {
            trackNoteCount++;
        }
    }

    ImGui::Text("Track %d Notes (%zu)", m_state.selectedTrackId, trackNoteCount);
    ImGui::Separator();

    if (ImGui::BeginTable("NotesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
                          ImVec2(0, 250))) {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30);
        ImGui::TableSetupColumn("Note(s)", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Volume", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableHeadersRow();

        static char editNoteBuffer[64] = "";
        static char editDurBuffer[16] = "";
        static float editVolume = 1.0f;

        int displayIndex = 0;
        for (size_t i = 0; i < m_state.notes.size(); i++) {
            Note& note = m_state.notes[i];

            // Filter: only show notes for selected track
            if (note.trackId != m_state.selectedTrackId) {
                continue;
            }

            displayIndex++;
            bool isSelected = (m_state.selectedNoteIndex == static_cast<int>(i));

            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));

            // Column 0: Row number (clickable to select)
            ImGui::TableNextColumn();
            if (ImGui::Selectable(std::to_string(displayIndex).c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                if (m_state.selectedNoteIndex != static_cast<int>(i)) {
                    // New selection - load values into edit buffers
                    m_state.selectedNoteIndex = static_cast<int>(i);
                    std::string noteStr;
                    for (size_t j = 0; j < note.noteNames.size(); j++) {
                        if (j > 0) noteStr += ",";
                        noteStr += note.noteNames[j];
                    }
                    strncpy(editNoteBuffer, noteStr.c_str(), sizeof(editNoteBuffer) - 1);
                    strncpy(editDurBuffer, note.durationStr.c_str(), sizeof(editDurBuffer) - 1);
                    editVolume = note.volume;
                } else {
                    // Deselect
                    m_state.selectedNoteIndex = -1;
                }
            }

            // Column 1: Note name(s)
            ImGui::TableNextColumn();
            if (isSelected) {
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##note", editNoteBuffer, sizeof(editNoteBuffer));
            } else {
                std::string noteStr;
                for (size_t j = 0; j < note.noteNames.size(); j++) {
                    if (j > 0) noteStr += ",";
                    noteStr += note.noteNames[j];
                }
                ImGui::Text("%s", noteStr.c_str());
            }

            // Column 2: Duration
            ImGui::TableNextColumn();
            if (isSelected) {
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##dur", editDurBuffer, sizeof(editDurBuffer));
            } else {
                ImGui::Text("%s", note.durationStr.c_str());
            }

            // Column 3: Volume
            ImGui::TableNextColumn();
            if (isSelected) {
                ImGui::SetNextItemWidth(-1);
                ImGui::DragFloat("##vol", &editVolume, 0.01f, 0.0f, 1.0f, "%.2f");
            } else {
                ImGui::Text("%.2f", note.volume);
            }

            // Column 4: Apply/Delete buttons for selected row
            ImGui::TableNextColumn();
            if (isSelected) {
                // Apply button (checkmark)
                if (ImGui::Button("OK")) {
                    std::string noteStr(editNoteBuffer);
                    std::string durStr(editDurBuffer);

                    // Trim whitespace
                    while (!noteStr.empty() && noteStr.back() == ' ') noteStr.pop_back();
                    while (!noteStr.empty() && noteStr.front() == ' ') noteStr.erase(0, 1);

                    bool valid = true;
                    std::vector<std::string> parsedNotes;

                    if (noteStr.empty()) {
                        m_state.setStatus("Error: Note name is empty");
                        valid = false;
                    } else if (durStr.empty()) {
                        m_state.setStatus("Error: Duration is empty");
                        valid = false;
                    } else if (noteStr == "R" || noteStr == "r") {
                        parsedNotes.push_back("R");
                    } else if (noteStr.find(',') != std::string::npos) {
                        // Parse chord
                        size_t start = 0;
                        size_t end;
                        while ((end = noteStr.find(',', start)) != std::string::npos) {
                            std::string n = noteStr.substr(start, end - start);
                            while (!n.empty() && n.front() == ' ') n.erase(0, 1);
                            while (!n.empty() && n.back() == ' ') n.pop_back();
                            if (!n.empty()) {
                                if (!NoteParser::isValidNote(n)) {
                                    m_state.setStatus("Error: Invalid note '" + n + "'");
                                    valid = false;
                                    break;
                                }
                                parsedNotes.push_back(n);
                            }
                            start = end + 1;
                        }
                        if (valid) {
                            std::string last = noteStr.substr(start);
                            while (!last.empty() && last.front() == ' ') last.erase(0, 1);
                            while (!last.empty() && last.back() == ' ') last.pop_back();
                            if (!last.empty()) {
                                if (!NoteParser::isValidNote(last)) {
                                    m_state.setStatus("Error: Invalid note '" + last + "'");
                                    valid = false;
                                } else {
                                    parsedNotes.push_back(last);
                                }
                            }
                        }
                    } else {
                        if (!NoteParser::isValidNote(noteStr)) {
                            m_state.setStatus("Error: Invalid note '" + noteStr + "'");
                            valid = false;
                        } else {
                            parsedNotes.push_back(noteStr);
                        }
                    }

                    // Validate duration
                    if (valid) {
                        float testDur = RhythmParser::parseRhythm(durStr, 120.0f, "4/4");
                        if (testDur <= 0) {
                            m_state.setStatus("Error: Invalid duration '" + durStr + "'");
                            valid = false;
                        }
                    }

                    if (valid && !parsedNotes.empty()) {
                        note.noteNames = parsedNotes;
                        note.durationStr = durStr;
                        note.volume = editVolume;
                        m_state.markDirty();
                        m_state.selectedNoteIndex = -1;
                        m_state.setStatus("Note updated");
                    }
                }
                ImGui::SameLine();
                // Delete button (X)
                if (ImGui::Button("X")) {
                    m_state.notes.erase(m_state.notes.begin() + i);
                    m_state.selectedNoteIndex = -1;
                    m_state.markDirty();
                    ImGui::PopID();
                    break;  // Exit loop since we modified the vector
                }
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    // Note input fields
    ImGui::Separator();
    ImGui::Text("Note Properties:");

    ImGui::SetNextItemWidth(120);
    ImGui::InputText("Note(s)", m_noteNameBuffer, sizeof(m_noteNameBuffer));
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Examples: C4, A#4, Bb3");
        ImGui::Text("Chords: C4,E4,G4");
        ImGui::Text("Rest: R");
        ImGui::EndTooltip();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputText("Duration", m_durationBuffer, sizeof(m_durationBuffer));
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("w=whole, h=half, q=quarter");
        ImGui::Text("e=eighth, s=sixteenth");
        ImGui::Text("Add . for dotted, t for triplet");
        ImGui::Text("Or use seconds: 0.5");
        ImGui::EndTooltip();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputFloat("Vol", &m_noteVolume, 0.05f, 0.1f, "%.2f")) {
        if (m_noteVolume < 0.0f) m_noteVolume = 0.0f;
        if (m_noteVolume > 1.0f) m_noteVolume = 1.0f;
    }

    // Buttons
    if (ImGui::Button("Add Note")) {
        std::string noteStr(m_noteNameBuffer);
        std::string durStr(m_durationBuffer);

        // Trim whitespace
        while (!noteStr.empty() && noteStr.back() == ' ') noteStr.pop_back();
        while (!noteStr.empty() && noteStr.front() == ' ') noteStr.erase(0, 1);

        if (noteStr.empty()) {
            m_state.setStatus("Error: Note name is empty");
        } else if (durStr.empty()) {
            m_state.setStatus("Error: Duration is empty");
        } else {
            Note newNote;
            bool validNotes = true;

            // Check if it's a rest
            if (noteStr == "R" || noteStr == "r") {
                newNote.noteNames.push_back("R");
            } else if (noteStr.find(',') != std::string::npos) {
                // Parse chord - validate each note
                size_t start = 0;
                size_t end;
                while ((end = noteStr.find(',', start)) != std::string::npos) {
                    std::string n = noteStr.substr(start, end - start);
                    // Trim
                    while (!n.empty() && n.front() == ' ') n.erase(0, 1);
                    while (!n.empty() && n.back() == ' ') n.pop_back();
                    if (!n.empty()) {
                        if (!NoteParser::isValidNote(n)) {
                            m_state.setStatus("Error: Invalid note '" + n + "'");
                            validNotes = false;
                            break;
                        }
                        newNote.noteNames.push_back(n);
                    }
                    start = end + 1;
                }
                if (validNotes) {
                    std::string last = noteStr.substr(start);
                    while (!last.empty() && last.front() == ' ') last.erase(0, 1);
                    while (!last.empty() && last.back() == ' ') last.pop_back();
                    if (!last.empty()) {
                        if (!NoteParser::isValidNote(last)) {
                            m_state.setStatus("Error: Invalid note '" + last + "'");
                            validNotes = false;
                        } else {
                            newNote.noteNames.push_back(last);
                        }
                    }
                }
            } else {
                // Single note - validate
                if (!NoteParser::isValidNote(noteStr)) {
                    m_state.setStatus("Error: Invalid note '" + noteStr + "'");
                    validNotes = false;
                } else {
                    newNote.noteNames.push_back(noteStr);
                }
            }

            // Validate duration
            if (validNotes) {
                float testDuration = RhythmParser::parseRhythm(durStr, 120.0f, "4/4");
                if (testDuration <= 0) {
                    m_state.setStatus("Error: Invalid duration '" + durStr + "'");
                    validNotes = false;
                }
            }

            if (validNotes && !newNote.noteNames.empty()) {
                newNote.durationStr = durStr;
                newNote.duration = 0.5f;  // Will be recalculated during generation
                newNote.volume = m_noteVolume;
                newNote.trackId = m_state.selectedTrackId;
                m_state.notes.push_back(newNote);
                m_state.markDirty();
                m_state.setStatus("Note added");
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear All")) {
        m_state.notes.clear();
        m_state.selectedNoteIndex = -1;
        m_state.markDirty();
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(Click row to edit inline)");
}

void Application::renderWaveformPanel() {
    ImGui::Text("Track %d Waveform", m_state.selectedTrackId);
    ImGui::Separator();

    ImVec2 canvas_size = ImVec2(ImGui::GetContentRegionAvail().x, 100);
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_pos,
                             ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                             IM_COL32(30, 30, 30, 255));

    // Get audio for selected track
    SoundSamples* displayAudio = m_state.getSelectedTrackAudio();

    if (displayAudio) {
        float* samples = displayAudio->getsamples();
        int sampleCount = displayAudio->getLength();

        if (sampleCount > 0) {
            int step = std::max(1, sampleCount / static_cast<int>(canvas_size.x));
            float centerY = canvas_pos.y + canvas_size.y / 2;

            for (int x = 0; x < static_cast<int>(canvas_size.x) - 1; x++) {
                int idx1 = x * step;
                int idx2 = (x + 1) * step;
                if (idx2 >= sampleCount) idx2 = sampleCount - 1;

                float y1 = centerY - samples[idx1] * (canvas_size.y / 2) * 0.9f;
                float y2 = centerY - samples[idx2] * (canvas_size.y / 2) * 0.9f;

                draw_list->AddLine(ImVec2(canvas_pos.x + x, y1),
                                   ImVec2(canvas_pos.x + x + 1, y2),
                                   IM_COL32(0, 255, 100, 255));
            }

            if (m_audioPlayer.isPlaying()) {
                float playheadX = canvas_pos.x +
                    (static_cast<float>(m_state.playbackPosition) / sampleCount) * canvas_size.x;
                draw_list->AddLine(ImVec2(playheadX, canvas_pos.y),
                                   ImVec2(playheadX, canvas_pos.y + canvas_size.y),
                                   IM_COL32(255, 100, 0, 255), 2.0f);
            }
        }
    } else {
        float centerY = canvas_pos.y + canvas_size.y / 2;
        draw_list->AddLine(ImVec2(canvas_pos.x, centerY),
                           ImVec2(canvas_pos.x + canvas_size.x, centerY),
                           IM_COL32(60, 60, 60, 255));
    }

    draw_list->AddRect(canvas_pos,
                       ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                       IM_COL32(100, 100, 100, 255));

    ImGui::Dummy(canvas_size);
}

void Application::renderStatusBar() {
    ImGui::Separator();
    ImGui::Text("%s", m_state.statusMessage.c_str());

    if (m_state.hasUnsavedChanges) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "[Modified]");
    }

    if (m_state.generatedAudio) {
        ImGui::SameLine();
        float duration = static_cast<float>(m_state.generatedAudio->getLength()) / m_state.sampleRate;
        ImGui::Text("| Duration: %.2fs | Samples: %d", duration, m_state.generatedAudio->getLength());
    }
}

void Application::renderTemplatesPanel() {
    ImGui::Separator();
    ImGui::Text("Templates");
    ImGui::Separator();

    if (ImGui::Button("Twinkle Twinkle", ImVec2(-1, 0))) {
        loadTemplate(0);
    }
    if (ImGui::Button("Mary Had a Lamb", ImVec2(-1, 0))) {
        loadTemplate(1);
    }
    if (ImGui::Button("Happy Birthday", ImVec2(-1, 0))) {
        loadTemplate(2);
    }
    if (ImGui::Button("Ode to Joy", ImVec2(-1, 0))) {
        loadTemplate(3);
    }
    if (ImGui::Button("C Major Scale", ImVec2(-1, 0))) {
        loadTemplate(4);
    }
    if (ImGui::Button("Simple Chord Prog", ImVec2(-1, 0))) {
        loadTemplate(5);
    }
}

void Application::loadTemplate(int templateIndex) {
    m_state.notes.clear();
    m_state.selectedNoteIndex = -1;
    m_audioPlayer.stop();
    m_state.playbackPosition = 0;

    // Ensure track 1 exists and select it
    if (m_state.songContext.getTracks().empty()) {
        m_state.songContext.defineTrack(1, m_state.defaultWaveType, 1.0f);
    }
    m_state.selectedTrackId = 1;

    auto addNote = [&](const std::string& noteName, const std::string& duration, float volume = 1.0f) {
        Note n;
        n.noteNames.push_back(noteName);
        n.durationStr = duration;
        n.duration = 0.5f;
        n.volume = volume;
        n.trackId = 1;
        m_state.notes.push_back(n);
    };

    auto addChord = [&](const std::vector<std::string>& notes, const std::string& duration, float volume = 1.0f) {
        Note n;
        n.noteNames = notes;
        n.durationStr = duration;
        n.duration = 0.5f;
        n.volume = volume;
        n.trackId = 1;
        m_state.notes.push_back(n);
    };

    switch (templateIndex) {
        case 0:  // Twinkle Twinkle Little Star (Complete)
            // Verse 1: "Twinkle twinkle little star"
            addNote("C4", "q"); addNote("C4", "q");
            addNote("G4", "q"); addNote("G4", "q");
            addNote("A4", "q"); addNote("A4", "q");
            addNote("G4", "h");
            // "How I wonder what you are"
            addNote("F4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("D4", "q"); addNote("D4", "q");
            addNote("C4", "h");
            // "Up above the world so high"
            addNote("G4", "q"); addNote("G4", "q");
            addNote("F4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("D4", "h");
            // "Like a diamond in the sky"
            addNote("G4", "q"); addNote("G4", "q");
            addNote("F4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("D4", "h");
            // "Twinkle twinkle little star"
            addNote("C4", "q"); addNote("C4", "q");
            addNote("G4", "q"); addNote("G4", "q");
            addNote("A4", "q"); addNote("A4", "q");
            addNote("G4", "h");
            // "How I wonder what you are"
            addNote("F4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("D4", "q"); addNote("D4", "q");
            addNote("C4", "h");
            break;

        case 1:  // Mary Had a Little Lamb (Complete)
            // "Mary had a little lamb"
            addNote("E4", "q"); addNote("D4", "q");
            addNote("C4", "q"); addNote("D4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("E4", "h");
            // "Little lamb, little lamb"
            addNote("D4", "q"); addNote("D4", "q");
            addNote("D4", "h");
            addNote("E4", "q"); addNote("G4", "q");
            addNote("G4", "h");
            // "Mary had a little lamb"
            addNote("E4", "q"); addNote("D4", "q");
            addNote("C4", "q"); addNote("D4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            addNote("E4", "q"); addNote("E4", "q");
            // "Its fleece was white as snow"
            addNote("D4", "q"); addNote("D4", "q");
            addNote("E4", "q"); addNote("D4", "q");
            addNote("C4", "h"); addNote("C4", "h");
            break;

        case 2:  // Happy Birthday (Complete)
            // "Happy birthday to you"
            addNote("G3", "e."); addNote("G3", "s");
            addNote("A3", "q"); addNote("G3", "q");
            addNote("C4", "q"); addNote("B3", "h");
            // "Happy birthday to you"
            addNote("G3", "e."); addNote("G3", "s");
            addNote("A3", "q"); addNote("G3", "q");
            addNote("D4", "q"); addNote("C4", "h");
            // "Happy birthday dear [name]"
            addNote("G3", "e."); addNote("G3", "s");
            addNote("G4", "q"); addNote("E4", "q");
            addNote("C4", "q"); addNote("B3", "q");
            addNote("A3", "h");
            // "Happy birthday to you"
            addNote("F4", "e."); addNote("F4", "s");
            addNote("E4", "q"); addNote("C4", "q");
            addNote("D4", "q"); addNote("C4", "h");
            break;

        case 3:  // Ode to Joy (Beethoven - Complete main theme)
            // Line 1
            addNote("E4", "q"); addNote("E4", "q");
            addNote("F4", "q"); addNote("G4", "q");
            addNote("G4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("D4", "q");
            // Line 2
            addNote("C4", "q"); addNote("C4", "q");
            addNote("D4", "q"); addNote("E4", "q");
            addNote("E4", "q."); addNote("D4", "e");
            addNote("D4", "h");
            // Line 3 (repeat of line 1)
            addNote("E4", "q"); addNote("E4", "q");
            addNote("F4", "q"); addNote("G4", "q");
            addNote("G4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("D4", "q");
            // Line 4
            addNote("C4", "q"); addNote("C4", "q");
            addNote("D4", "q"); addNote("E4", "q");
            addNote("D4", "q."); addNote("C4", "e");
            addNote("C4", "h");
            // Line 5 (bridge)
            addNote("D4", "q"); addNote("D4", "q");
            addNote("E4", "q"); addNote("C4", "q");
            addNote("D4", "q"); addNote("E4", "e"); addNote("F4", "e");
            addNote("E4", "q"); addNote("C4", "q");
            // Line 6
            addNote("D4", "q"); addNote("E4", "e"); addNote("F4", "e");
            addNote("E4", "q"); addNote("D4", "q");
            addNote("C4", "q"); addNote("D4", "q");
            addNote("G3", "h");
            // Final lines (return to main theme)
            addNote("E4", "q"); addNote("E4", "q");
            addNote("F4", "q"); addNote("G4", "q");
            addNote("G4", "q"); addNote("F4", "q");
            addNote("E4", "q"); addNote("D4", "q");
            addNote("C4", "q"); addNote("C4", "q");
            addNote("D4", "q"); addNote("E4", "q");
            addNote("D4", "q."); addNote("C4", "e");
            addNote("C4", "h");
            break;

        case 4:  // C Major Scale (Two octaves up and down)
            // First octave up
            addNote("C4", "e"); addNote("D4", "e");
            addNote("E4", "e"); addNote("F4", "e");
            addNote("G4", "e"); addNote("A4", "e");
            addNote("B4", "e"); addNote("C5", "e");
            // Second octave up
            addNote("D5", "e"); addNote("E5", "e");
            addNote("F5", "e"); addNote("G5", "e");
            addNote("A5", "e"); addNote("B5", "e");
            addNote("C6", "q");
            // Pause at top
            addNote("C6", "q");
            // Second octave down
            addNote("B5", "e"); addNote("A5", "e");
            addNote("G5", "e"); addNote("F5", "e");
            addNote("E5", "e"); addNote("D5", "e");
            addNote("C5", "e"); addNote("B4", "e");
            // First octave down
            addNote("A4", "e"); addNote("G4", "e");
            addNote("F4", "e"); addNote("E4", "e");
            addNote("D4", "e"); addNote("C4", "e");
            addNote("C4", "h");
            break;

        case 5:  // Complete Chord Progression (I-V-vi-IV with variations)
            // First progression: C-G-Am-F (I-V-vi-IV)
            addChord({"C4", "E4", "G4"}, "h");      // C major
            addChord({"G3", "B3", "D4"}, "h");      // G major
            addChord({"A3", "C4", "E4"}, "h");      // A minor
            addChord({"F3", "A3", "C4"}, "h");      // F major
            // Second progression with inversions
            addChord({"C4", "E4", "G4"}, "h");      // C major
            addChord({"B3", "D4", "G4"}, "h");      // G major (1st inv)
            addChord({"C4", "E4", "A4"}, "h");      // A minor (1st inv)
            addChord({"C4", "F4", "A4"}, "h");      // F major (2nd inv)
            // Third progression: Am-F-C-G (vi-IV-I-V)
            addChord({"A3", "C4", "E4"}, "h");      // A minor
            addChord({"F3", "A3", "C4"}, "h");      // F major
            addChord({"C4", "E4", "G4"}, "h");      // C major
            addChord({"G3", "B3", "D4"}, "h");      // G major
            // Final cadence
            addChord({"F3", "A3", "C4"}, "q");      // F major
            addChord({"G3", "B3", "D4"}, "q");      // G major
            addChord({"C3", "E3", "G3", "C4"}, "w"); // C major (full)
            break;
    }

    m_state.markDirty();
    m_state.setStatus("Template loaded");
}

void Application::generateAudio() {
    // Stop any currently playing audio to reset player state
    m_audioPlayer.stop();
    m_state.playbackPosition = 0;

    if (m_state.notes.empty()) {
        m_state.setStatus("No notes to generate");
        return;
    }

    m_state.setStatus("Generating audio...");

    // Parse duration strings to actual durations using tempo and time signature
    float tempo = m_state.songContext.getTempo();
    std::string timeSig = m_state.songContext.getTimeSignature();

    std::map<int, std::vector<Note>> trackNotes;
    for (Note note : m_state.notes) {
        // Convert durationStr (like "q", "h", "e.") to duration in seconds
        note.duration = RhythmParser::parseRhythm(note.durationStr, tempo, timeSig);
        if (note.duration <= 0) {
            // Fallback for invalid duration strings
            note.duration = 0.5f;
        }
        trackNotes[note.trackId].push_back(note);
    }

    // Clear previous per-track audio
    m_state.trackAudio.clear();

    std::map<int, SoundSamples*> rawTrackAudio;
    bool hasErrors = false;
    for (auto& pair : trackNotes) {
        int trackId = pair.first;
        std::vector<Note>& notes = pair.second;

        Track trackInfo = m_state.songContext.getTrackInfo(trackId, m_state.defaultWaveType);
        SoundSamples* audio = SongGenerator::generateTrackAudio(notes, trackInfo, m_state.sampleRate);
        if (audio) {
            rawTrackAudio[trackId] = audio;
            // Store a copy for per-track playback
            SoundSamples* copy = new SoundSamples(audio->getLength(), audio->getSampleRate());
            float* src = audio->getsamples();
            float* dst = copy->getsamples();
            for (int i = 0; i < audio->getLength(); i++) {
                dst[i] = src[i];
            }
            m_state.trackAudio[trackId].reset(copy);
        } else {
            hasErrors = true;
        }
    }

    if (rawTrackAudio.empty()) {
        m_state.setStatus("Error: Failed to generate any audio");
        return;
    }

    SoundSamples* mixed = SongGenerator::mixTracks(rawTrackAudio, m_state.sampleRate);

    for (auto& pair : rawTrackAudio) {
        delete pair.second;
    }

    if (!mixed || mixed->getLength() == 0) {
        m_state.setStatus("Error: Failed to mix audio tracks");
        if (mixed) delete mixed;
        return;
    }

    m_state.generatedAudio.reset(mixed);
    m_state.audioNeedsRegeneration = false;

    if (hasErrors) {
        m_state.setStatus("Audio generated with some errors (check notes)");
    } else {
        m_state.setStatus("Audio generated successfully");
    }
}

void Application::exportWav(const std::string& path) {
    if (!m_state.generatedAudio) {
        m_state.setStatus("Error: No audio to export");
        return;
    }

    if (path.empty()) {
        m_state.setStatus("Error: No export path specified");
        return;
    }

    bool success = WavExporter::exportToWav(m_state.generatedAudio.get(), path, 16);

    if (success) {
        m_state.setStatus("Exported WAV to " + path);
    } else {
        m_state.setStatus("Error: Could not export WAV file");
    }
}

void Application::openMusicXMLFile() {
    // Simple file path input using ImGui popup
    static char pathBuffer[512] = "";
    static bool showOpenPopup = false;

    if (!showOpenPopup) {
        showOpenPopup = true;
        ImGui::OpenPopup("Open MusicXML File");
    }

    if (ImGui::BeginPopupModal("Open MusicXML File", &showOpenPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter file path:");
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##openfilepath", pathBuffer, sizeof(pathBuffer));

        ImGui::Spacing();

        if (ImGui::Button("Open", ImVec2(120, 0))) {
            std::string filepath = pathBuffer;
            if (!filepath.empty()) {
                // Clear current state
                m_state.notes.clear();
                m_state.selectedNoteIndex = -1;
                m_audioPlayer.stop();
                m_state.playbackPosition = 0;
                m_state.trackAudio.clear();
                m_state.generatedAudio.reset();

                // Reset context
                SongContext newContext;
                newContext.setTempo(120.0f);

                // Load file
                std::vector<Note> loadedNotes = MusicXMLParser::readFromFile(filepath, newContext);

                if (!loadedNotes.empty() || !newContext.getTracks().empty()) {
                    m_state.notes = loadedNotes;
                    m_state.songContext = newContext;
                    m_state.currentFilePath = filepath;
                    m_state.hasUnsavedChanges = false;
                    m_state.audioNeedsRegeneration = true;

                    // Select first track if available
                    if (!m_state.songContext.getTracks().empty()) {
                        m_state.selectedTrackId = m_state.songContext.getTracks().begin()->first;
                    }

                    m_state.setStatus("Loaded: " + filepath);
                } else {
                    m_state.setStatus("Error: Could not load MusicXML file");
                }
            }
            showOpenPopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showOpenPopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Application::saveMusicXMLFile() {
    if (m_state.currentFilePath.empty()) {
        saveMusicXMLFileAs();
        return;
    }

    bool success = MusicXMLParser::writeToFile(
        m_state.notes,
        m_state.songContext,
        m_state.currentFilePath
    );

    if (success) {
        m_state.hasUnsavedChanges = false;
        m_state.setStatus("Saved: " + m_state.currentFilePath);
    } else {
        m_state.setStatus("Error: Could not save file");
    }
}

void Application::saveMusicXMLFileAs() {
    static char pathBuffer[512] = "song.musicxml";
    static bool showSavePopup = false;

    if (!showSavePopup) {
        showSavePopup = true;
        ImGui::OpenPopup("Save MusicXML File");
    }

    if (ImGui::BeginPopupModal("Save MusicXML File", &showSavePopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter file path:");
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##savefilepath", pathBuffer, sizeof(pathBuffer));

        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            std::string filepath = pathBuffer;
            if (!filepath.empty()) {
                // Ensure .musicxml extension
                if (filepath.find(".musicxml") == std::string::npos &&
                    filepath.find(".xml") == std::string::npos) {
                    filepath += ".musicxml";
                }

                bool success = MusicXMLParser::writeToFile(
                    m_state.notes,
                    m_state.songContext,
                    filepath
                );

                if (success) {
                    m_state.currentFilePath = filepath;
                    m_state.hasUnsavedChanges = false;
                    m_state.setStatus("Saved: " + filepath);
                } else {
                    m_state.setStatus("Error: Could not save file");
                }
            }
            showSavePopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showSavePopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
