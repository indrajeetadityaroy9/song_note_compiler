#ifndef SONG_NOTE_COMPILER_SOUNDSAMPLES_H
#define SONG_NOTE_COMPILER_SOUNDSAMPLES_H

class SoundSamples {
private:
    float sample_rate;
    int length;
    float *samples;

public:
    SoundSamples();
    SoundSamples(const float *samples, int length, float sample_rate);
    SoundSamples(int length, float sample_rate);
    SoundSamples(const SoundSamples &S);
    SoundSamples(SoundSamples&& S) noexcept;
    ~SoundSamples();

    float getSampleRate() const;
    int getLength() const;
    float *getsamples() const;

    SoundSamples &operator=(const SoundSamples &S);
    SoundSamples &operator=(SoundSamples&& S) noexcept;
    float &operator[](int index) const;
    SoundSamples operator+(const SoundSamples &S) const;
};

#endif
