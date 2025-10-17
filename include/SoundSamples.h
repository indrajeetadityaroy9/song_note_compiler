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
    float getSampleRate() const;
    int getLength() const;
    float *getsamples() const;
    SoundSamples &operator=(const SoundSamples &S);
    float &operator[](int index) const;
    SoundSamples operator+(const SoundSamples &S) const;
    void adsr( float atime, float alevel, float dtime, float slevel, float rtime);
};

#endif
