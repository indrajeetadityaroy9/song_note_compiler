# Song Note Compiler

A music generation tool for composing and synthesizing audio from musical notation. Features real-time playback, waveform visualization, and WAV export.

## Features

- **Interactive GUI** with real-time audio playback
- **Multiple waveform types**: Sine, Square, Triangle, and Sawtooth
- **Standard note notation**: C4, A#4, Bb3, etc. (C0-B8 range)
- **Chord support**: Multiple notes played simultaneously
- **Rhythmic notation**: Whole, half, quarter, eighth, sixteenth notes
- **Dotted and triplet rhythms**
- **Multi-track composition** with per-track wave type and volume
- **ADSR envelope** for natural-sounding notes
- **Template songs** for quick starting points
- **Waveform visualization** with playhead
- **WAV file export** (16-bit PCM, 44.1kHz)

## Building

### Requirements

- C++17 compiler (g++ or clang++)
- GLFW 3.x (`brew install glfw` on macOS, `apt install libglfw3-dev` on Linux)
- OpenGL 3.2+

### Build

```bash
make        # Build the application
make clean  # Remove build artifacts
make help   # Show available targets
```

### Run

```bash
./bin/songgen-gui
```

## Usage

### Track Panel (Left)

- **Time Signature**: Select 4/4, 3/4, 2/4, or 6/8
- **Track List**: Click to select a track
  - Wave type selector (Sine, Square, Triangle, Sawtooth)
  - Volume slider (0.0 - 1.0)
  - Delete track button
- **Add Track**: Create new tracks with selected wave type

### Templates

Pre-loaded songs to get started quickly:
- Twinkle Twinkle Little Star
- Mary Had a Little Lamb
- Happy Birthday
- Ode to Joy (Beethoven)
- C Major Scale (2 octaves)
- Chord Progression (I-V-vi-IV)

### Notes Editor (Right)

Shows notes for the **selected track**:
- Click a row to edit inline
- OK to apply changes, X to delete
- Add notes using the input fields below

### Note Input

| Field | Format | Examples |
|-------|--------|----------|
| Note(s) | Note name + octave | `C4`, `A#4`, `Bb3` |
| Chords | Comma-separated | `C4,E4,G4` |
| Rest | R | `R` |
| Duration | Rhythmic or seconds | `q`, `h.`, `0.5` |
| Volume | 0.0 - 1.0 | `0.8` |

### Duration Notation

| Symbol | Name | Beats |
|--------|------|-------|
| `w` | Whole note | 4 |
| `h` | Half note | 2 |
| `q` | Quarter note | 1 |
| `e` | Eighth note | 0.5 |
| `s` | Sixteenth note | 0.25 |
| `.` | Dotted | +50% duration |
| `t` | Triplet | 2/3 duration |

Examples: `q` (quarter), `h.` (dotted half), `et` (eighth triplet)

### Transport Controls

| Button | Action |
|--------|--------|
| **Play** | Play selected track |
| **Play All** | Play all tracks mixed |
| **Pause** | Pause playback |
| **Generate** | Generate audio from notes |
| **Export WAV** | Save to WAV file |

### Waveform Panel

Displays the waveform for the selected track with:
- Green waveform visualization
- Orange playhead during playback

## Wave Types

| Type | Sound Character |
|------|-----------------|
| **Sine** | Smooth, pure tone |
| **Square** | Harsh, 8-bit retro |
| **Triangle** | Mellow, soft |
| **Sawtooth** | Bright, buzzy |


## Dependencies (Included)

- **Dear ImGui**: Immediate-mode GUI library
- **miniaudio**: Single-header audio playback library
- **AudioFile**: WAV file writing library
