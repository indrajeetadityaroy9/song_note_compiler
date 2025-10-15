# Song Note Compiler
A music generation tool that compiles musical notation files into audio waveforms. Uses object-oriented design with polymorphic wave generation to synthesize songs and instrumentals.

## Features
- Generate songs from simple text-based musical notation
- Multiple waveform types: Sine, Square, Triangle, and Sawtooth
- Supports standard note names (C4, A#4, Bb3, etc.)
- Configurable sample rate and output format
- WAV file export for immediate playback

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
- One note per line: `NoteName Duration`
- Lines starting with `#` are comments
- Note names: C4, D#4, Eb5, etc. (octaves 0-8)
- Duration in seconds

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
- **--wav**: (optional) Output in WAV format instead of raw text

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


## Musical Note Frequencies

| Note | Frequency (Hz) |
|------|----------------|
| C4   | 261.63         |
| D4   | 293.66         |
| E4   | 329.63         |
| F4   | 349.23         |
| G4   | 392.00         |
| A4   | 440.00         |
