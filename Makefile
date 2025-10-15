CXX = g++
CXXFLAGS = -g -Wall -std=c++17 -Iinclude
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/songgen

# Object files for songgen
OBJECTS = $(BUILD_DIR)/songgen.o \
          $(BUILD_DIR)/NoteParser.o \
          $(BUILD_DIR)/SawtoothWave.o \
          $(BUILD_DIR)/SineWave.o \
          $(BUILD_DIR)/SquareWave.o \
          $(BUILD_DIR)/TriangleWave.o \
          $(BUILD_DIR)/SoundSamples.o \
          $(BUILD_DIR)/soundio.o \
          $(BUILD_DIR)/wave.o \
          $(BUILD_DIR)/WavExporter.o

# All source files (for compilation rules)
SOURCES = $(SRC_DIR)/songgen.cpp \
          $(SRC_DIR)/NoteParser.cpp \
          $(SRC_DIR)/SawtoothWave.cpp \
          $(SRC_DIR)/SineWave.cpp \
          $(SRC_DIR)/SquareWave.cpp \
          $(SRC_DIR)/TriangleWave.cpp \
          $(SRC_DIR)/SoundSamples.cpp \
          $(SRC_DIR)/soundio.cpp \
          $(SRC_DIR)/wave.cpp \
          $(SRC_DIR)/WavExporter.cpp

HEADERS = $(INC_DIR)/SawtoothWave.h \
          $(INC_DIR)/SineWave.h \
          $(INC_DIR)/SquareWave.h \
          $(INC_DIR)/TriangleWave.h \
          $(INC_DIR)/SoundSamples.h \
          $(INC_DIR)/soundio.h \
          $(INC_DIR)/wave.h \
          $(INC_DIR)/WavExporter.h \
          $(INC_DIR)/AudioFile.h \
          $(INC_DIR)/NoteParser.h

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f *.o

distclean: clean
	rm -rf test_notes twinkle_notes
	rm -f *.raw *.wav

install: $(TARGET)
	@echo "To install system-wide, run: sudo cp $(TARGET) /usr/local/bin/"
	@echo "Current executable is at: $(TARGET)"

help:
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  distclean  - Remove build artifacts and test outputs"
	@echo "  install    - Show installation instructions"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage: make [target]"

.PHONY: all clean distclean install help
