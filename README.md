# Song Note Compiler
A music generation tool that compiles musical notation files into audio waveforms. Uses object-oriented design with polymorphic wave generation to synthesize songs and instrumentals.

## Features
- Generate songs from simple text-based musical notation
- Multiple waveform types: Sine, Square, Triangle, and Sawtooth
- Supports standard note names (C4, A#4, Bb3, etc.)
- Configurable sample rate and output format
- WAV file export for immediate playback
- Chord support (multiple notes played simultaneously)
- Rests/pauses in music
- Volume control for individual notes/chords
- Rhythmic notation (quarter notes, eighth notes, dotted notes, triplets)
- Tempo changes (BPM)
- Time signature support
- Multi-track composition with different instruments
- More complex musical arrangements

## Usage

Generate audio from musical notation files:
```bash
bin/songgen <input_file> <wave_type> <sample_rate> <output_file> [--wav]
```

**Example:**
```bash
bin/songgen twinkle_twinkle.txt 1 44100 twinkle.wav --wav
```

**Input file format:**
- One note/chord per line: `NoteName Duration [Volume]`
- Lines starting with `#` are comments
- Single notes: `C4 0.5` or `C4 q`
- Chords: `C4,E4,G4 0.5` or `C4,E4,G4 q` (comma-separated notes)
- Rests: `R 0.5` or `R q` (silence)
- Volume: Optional third parameter 0.0-1.0 (default: 1.0)
- Tempo changes: `TEMPO 120` (sets tempo to 120 BPM)
- Time signature: `TIME 4/4` (sets time signature to 4/4)
- Track definitions: `TRACK <track_id> <wave_type> <volume>`
- Track switching: `> <track_id>`

**Duration formats:**
- Numeric: `0.5` (seconds)
- Rhythmic notation:
  - `w`  = whole note (4 beats)
  - `h`  = half note (2 beats)
  - `q`  = quarter note (1 beat)
  - `e`  = eighth note (1/2 beat)
  - `s`  = sixteenth note (1/4 beat)
  - `.`  = dotted (increases duration by 50%)
  - `t`  = triplet (decreases duration to 2/3)
- Examples: `q` (quarter note), `h.` (dotted half note), `8t` (eighth note triplet)

**Example notation file:**
```
# Twinkle Twinkle Little Star
C4 0.5
C4 0.5
G4 0.5
G4 0.5
A4 0.5
A4 0.5
G4 1.0
```

### Parameters

- **input_file**: Text file with musical notation
- **wave_type**: Wave type selector
  - `1` = Sine Wave (smooth, pure tone)
  - `2` = Square Wave (harsh, 8-bit sound)
  - `3` = Triangle Wave (mellow, soft)
  - `4` = Sawtooth Wave (bright, buzzy)
- **sample_rate**: Sample rate in Hz (e.g., 44100 for CD quality)
- **output_file**: Path to save the audio file
- **wav**: (optional) Output in WAV format instead of raw text

### Examples

Generate "Twinkle Twinkle Little Star" (24 seconds, 42 notes):
```bash
bin/songgen twinkle_twinkle.txt 1 44100 twinkle.wav --wav
```

**Generate with different waveforms:**
```bash
bin/songgen twinkle_twinkle.txt 1 44100 twinkle_sine.wav --wav      # Sine: Smooth
bin/songgen twinkle_twinkle.txt 2 44100 twinkle_square.wav --wav    # Square: 8-bit style
bin/songgen twinkle_twinkle.txt 3 44100 twinkle_triangle.wav --wav  # Triangle: Mellow
bin/songgen twinkle_twinkle.txt 4 44100 twinkle_sawtooth.wav --wav  # Sawtooth: Bright
```
