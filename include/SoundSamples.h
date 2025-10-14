#ifndef SONG_NOTE_COMPILER_SOUNDSAMPLES_H
#define SONG_NOTE_COMPILER_SOUNDSAMPLES_H

class SoundSamples {
private:
    /**
     * sample rate
     */
    float sample_rate;
    /**
     * number of samples in the sequence.
     */
    int length;
    /**
     * sequence of samples
     */
    float *samples;

public:
    /**
     * default constructor
     */
    SoundSamples();

    /**
     * specific constructor that takes float array, no. of samples, samplerate
     * @param samples
     * @param length
     * @param sample_rate
     */
    SoundSamples(const float *samples, int length, float sample_rate);

    /**
     * specific constructor that takes no. of samples and samplerate
     * @param length
     * @param sample_rate
     */
    SoundSamples(int length, float sample_rate);

    /**
     * copy constructor that deep copies the object
     * @param S
     */
    SoundSamples(const SoundSamples &S);

    /**
     * getter function for sample rate
     * @return sample rate
     */
    float getSampleRate() const;

    /**
     * getter function for no. of samples
     * @return no. of samples
     */
    int getLength() const;

    /**
     * getter function for samples array
     * @return samples array
     */
    float *getsamples() const;

    /**
     * Overloaded = operator
     * @param S
     * @return
     */
    SoundSamples &operator=(const SoundSamples &S);

    /**
     * Overloaded [] operator
     * @param index
     * @return
     */
    float &operator[](int index) const;

    /**
     * Overloaded + operator
     * @param S
     * @return
     */
    SoundSamples operator+(const SoundSamples &S) const;

    /**
     *
     * @param atime
     * @param alevel
     * @param dtime
     * @param slevel
     * @param rtime
     */
    void adsr( float atime, float alevel, float dtime, float slevel, float rtime);
};

#endif //SONG_NOTE_COMPILER_SOUNDSAMPLES_H
