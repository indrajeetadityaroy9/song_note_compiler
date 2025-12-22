CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -Iinclude -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/miniaudio
LDFLAGS = -lm

# Platform-specific flags for GUI
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null || echo "-I/opt/homebrew/include")
    GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null || echo "-L/opt/homebrew/lib -lglfw")
    GL_LIBS := -framework OpenGL
else
    GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
    GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null)
    GL_LIBS := -lGL
endif

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
VENDOR_DIR = vendor

# Target
TARGET = $(BIN_DIR)/songgen-gui

# Shared audio/wave object files
AUDIO_OBJECTS = $(BUILD_DIR)/NoteParser.o \
                $(BUILD_DIR)/RhythmParser.o \
                $(BUILD_DIR)/SawtoothWave.o \
                $(BUILD_DIR)/SineWave.o \
                $(BUILD_DIR)/SquareWave.o \
                $(BUILD_DIR)/TriangleWave.o \
                $(BUILD_DIR)/SoundSamples.o \
                $(BUILD_DIR)/soundio.o \
                $(BUILD_DIR)/wave.o \
                $(BUILD_DIR)/WavExporter.o \
                $(BUILD_DIR)/Envelope.o

# Core module objects
CORE_OBJECTS = $(BUILD_DIR)/core/SongGenerator.o \
               $(BUILD_DIR)/core/FileParser.o

# GUI objects
GUI_OBJECTS = $(BUILD_DIR)/gui/main_gui.o \
              $(BUILD_DIR)/gui/Application.o \
              $(BUILD_DIR)/gui/AudioPlayer.o \
              $(BUILD_DIR)/gui/miniaudio_impl.o

# ImGui objects
IMGUI_OBJECTS = $(BUILD_DIR)/imgui/imgui.o \
                $(BUILD_DIR)/imgui/imgui_draw.o \
                $(BUILD_DIR)/imgui/imgui_tables.o \
                $(BUILD_DIR)/imgui/imgui_widgets.o \
                $(BUILD_DIR)/imgui/imgui_impl_glfw.o \
                $(BUILD_DIR)/imgui/imgui_impl_opengl3.o

# Headers
HEADERS = $(INC_DIR)/SawtoothWave.h \
          $(INC_DIR)/SineWave.h \
          $(INC_DIR)/SquareWave.h \
          $(INC_DIR)/TriangleWave.h \
          $(INC_DIR)/SoundSamples.h \
          $(INC_DIR)/soundio.h \
          $(INC_DIR)/wave.h \
          $(INC_DIR)/WavExporter.h \
          $(INC_DIR)/AudioFile.h \
          $(INC_DIR)/NoteParser.h \
          $(INC_DIR)/RhythmParser.h \
          $(INC_DIR)/Envelope.h \
          $(INC_DIR)/core/Note.h \
          $(INC_DIR)/core/Track.h \
          $(INC_DIR)/core/SongContext.h \
          $(INC_DIR)/core/SongGenerator.h \
          $(INC_DIR)/core/FileParser.h \
          $(INC_DIR)/gui/EditorState.h \
          $(INC_DIR)/gui/AudioPlayer.h \
          $(INC_DIR)/gui/Application.h

# Default target: build GUI
all: $(TARGET)

$(TARGET): $(AUDIO_OBJECTS) $(CORE_OBJECTS) $(GUI_OBJECTS) $(IMGUI_OBJECTS) | $(BIN_DIR)
	$(CXX) $(AUDIO_OBJECTS) $(CORE_OBJECTS) $(GUI_OBJECTS) $(IMGUI_OBJECTS) \
	    -o $(TARGET) $(LDFLAGS) $(GLFW_LIBS) $(GL_LIBS) -lpthread

# Audio/wave compilation rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directories exist
DIRS = $(BUILD_DIR) $(BUILD_DIR)/core $(BUILD_DIR)/gui $(BUILD_DIR)/imgui $(BIN_DIR)

$(DIRS):
	mkdir -p $@

# Core compilation rules
$(BUILD_DIR)/core/SongGenerator.o: $(SRC_DIR)/core/SongGenerator.cpp $(HEADERS) | $(BUILD_DIR)/core
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/core/FileParser.o: $(SRC_DIR)/core/FileParser.cpp $(HEADERS) | $(BUILD_DIR)/core
	$(CXX) $(CXXFLAGS) -c $< -o $@

# GUI compilation rules
$(BUILD_DIR)/gui/main_gui.o: $(SRC_DIR)/gui/main_gui.cpp $(INC_DIR)/gui/Application.h | $(BUILD_DIR)/gui
	$(CXX) $(CXXFLAGS) $(GLFW_CFLAGS) -c $< -o $@

$(BUILD_DIR)/gui/Application.o: $(SRC_DIR)/gui/Application.cpp $(INC_DIR)/gui/Application.h $(INC_DIR)/gui/EditorState.h $(INC_DIR)/gui/AudioPlayer.h | $(BUILD_DIR)/gui
	$(CXX) $(CXXFLAGS) $(GLFW_CFLAGS) -c $< -o $@

$(BUILD_DIR)/gui/AudioPlayer.o: $(SRC_DIR)/gui/AudioPlayer.cpp $(INC_DIR)/gui/AudioPlayer.h | $(BUILD_DIR)/gui
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/gui/miniaudio_impl.o: $(SRC_DIR)/gui/miniaudio_impl.cpp | $(BUILD_DIR)/gui
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ImGui compilation rules
$(BUILD_DIR)/imgui/imgui.o: $(VENDOR_DIR)/imgui/imgui.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui/imgui_draw.o: $(VENDOR_DIR)/imgui/imgui_draw.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui/imgui_tables.o: $(VENDOR_DIR)/imgui/imgui_tables.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui/imgui_widgets.o: $(VENDOR_DIR)/imgui/imgui_widgets.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui/imgui_impl_glfw.o: $(VENDOR_DIR)/imgui/backends/imgui_impl_glfw.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) $(GLFW_CFLAGS) -c $< -o $@

$(BUILD_DIR)/imgui/imgui_impl_opengl3.o: $(VENDOR_DIR)/imgui/backends/imgui_impl_opengl3.cpp | $(BUILD_DIR)/imgui
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f *.o

distclean: clean
	rm -f *.raw *.wav

install: $(TARGET)
	@echo "To install system-wide, run: sudo cp $(TARGET) /usr/local/bin/songgen"
	@echo "Current executable is at: $(TARGET)"

help:
	@echo "Song Note Compiler - GUI Application"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the application (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  distclean  - Remove build artifacts and audio outputs"
	@echo "  install    - Show installation instructions"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Requirements: GLFW3, OpenGL"

.PHONY: all clean distclean install help
