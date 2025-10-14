# Sound Processor and Song Note Compiler
A digital sound processing tool that generates waveforms and manipulates audio samples using object-oriented design with polymorphic wave generation.

## Features
- Generate multiple waveform types: Sine, Square, Triangle, and Sawtooth
- Configurable sample rate, frequency, and duration
- Custom operator overloading for audio signal manipulation
- Modular architecture with clean separation of concerns

## Usage

### Generate Single Notes

Generate a waveform:
```bash
bin/genwave <wave_type> <sample_rate> <frequency> <duration> <output_file> [--wav]
```

### Generate Melodies

Generate "Twinkle Twinkle Little Star" as a single audio file:
```bash
bin/melody_generator <wave_type> <sample_rate> <output_file> [--wav]
```

Example:
```bash
bin/melody_generator 1 44100 twinkle.wav --wav
```

### Parameters

- **wave_type**: Wave type selector
  - `1` = Sine Wave (smooth, pure tone)
  - `2` = Square Wave (harsh, 8-bit sound)
  - `3` = Triangle Wave (mellow, soft)
  - `4` = Sawtooth Wave (bright, buzzy)
- **sample_rate**: Sample rate in Hz (e.g., 44100 for CD quality)
- **frequency**: Note frequency in Hz (e.g., 440 for A4)
- **duration**: Length in seconds (e.g., 0.5, 1.0)
- **output_file**: Path to save the audio file
- **--wav**: (optional) Output in WAV format instead of raw text

### Examples

Generate a 440Hz sine wave (A4 note) for 1 second as WAV:
```bash
bin/genwave 1 44100 440 1.0 output.wav --wav
```

Generate middle C (261.63Hz) using square wave for 0.5 seconds as raw text:
```bash
bin/genwave 2 44100 261.63 0.5 middle_c.raw
```

Generate complete "Twinkle Twinkle Little Star" melody (24 seconds, single file):
```bash
bin/melody_generator 1 44100 twinkle.wav --wav
```

## Output Formats

### WAV Format
Use the `--wav` flag to generate playable WAV audio files:
- Standard WAV format, playable in any media player
- 16-bit PCM encoding
- Mono channel
- No external conversion needed

### Raw Text Format
Without the `--wav` flag, files are generated in raw text format:
- One floating-point sample per line
- Values range from -1.0 to 1.0
- Sample count = `sample_rate * duration`
- Requires external tools to convert to playable audio

## Testing

Generate the complete melody:
```bash
bin/melody_generator 1 44100 test.wav --wav
```

Test different wave types:
```bash
bin/melody_generator 1 44100 sine.wav --wav      # Smooth
bin/melody_generator 2 44100 square.wav --wav    # 8-bit style
bin/melody_generator 3 44100 triangle.wav --wav  # Mellow
bin/melody_generator 4 44100 sawtooth.wav --wav  # Bright
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
