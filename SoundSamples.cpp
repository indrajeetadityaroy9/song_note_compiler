#include <iostream>
#include "SoundSamples.h"

using namespace std;

/**
 * default constructor
 */
SoundSamples::SoundSamples() {
    length = 0;
    sample_rate = 8000;
}

/**
 * specific constructor that takes float array, no. of samples, samplerate
 * @param samples
 * @param length
 * @param sample_rate
 */
SoundSamples::SoundSamples(const float *samples, int length, float sample_rate) {
    this->length = length;
    this->sample_rate = sample_rate;
    this->samples = (float *) malloc(length * sizeof(float));

    for (int i = 0; i < length; i++) {
        this->samples[i] = samples[i];
    }
}

/**
 * specific constructor that takes no. of samples and samplerate
 * @param length
 * @param sample_rate
 */
SoundSamples::SoundSamples(int length, float sample_rate) {
    this->length = length;
    this->sample_rate = sample_rate;
    this->samples = (float *) malloc(length * sizeof(float));

    for (int i = 0; i < length; i++) {
        this->samples[i] = 0;
    }
}

/**
 * copy constructor that deep copies the object
 * @param S
 */
SoundSamples::SoundSamples(const SoundSamples &S) {
    if (this != &S) {
        delete[] samples;
        samples = (float *) malloc(S.length * sizeof(float));
        length = S.length;
        sample_rate = S.sample_rate;

        for (int i = 0; i < S.length; i++) {
            this->samples[i] = S.samples[i];
        }
    }
}

/**
 * Overloaded = operator
 * @param S
 * @return
 */
SoundSamples &SoundSamples::operator=(const SoundSamples &S) {
    if (this != &S) {
        delete[] samples;
        samples = (float *) malloc(S.length * sizeof(float));
        length = S.length;
        sample_rate = S.sample_rate;

        for (int i = 0; i < S.length; i++) {
            this->samples[i] = S.samples[i];
        }
    }
    return *this;
}

/**
 * Overloaded + operator
 * @param S
 * @return
 */
SoundSamples SoundSamples::operator+(const SoundSamples &S) const {
    SoundSamples a;
    a.sample_rate = S.sample_rate;
    a.length = this->length + S.length;
    a.samples = (float *) malloc(a.length * sizeof(float));

    int index = 0;
    for (int i = 0; i < this->length; i++) {
        a.samples[i] = this->samples[index++];
    }
    index = 0;

    for (int j = this->length; j < a.length; j++) {
        a.samples[j] = S.samples[index++];
    }

    return a;
}

/**
 * Overloaded [] operator
 * @param index
 * @return
 */
float &SoundSamples::operator[](int index) const {
    if (index >= length) {
        cout << "Array index out of bound, exiting";
        exit(0);
    }
    return samples[index];
}

/**
 * getter function for sample rate
 * @return
 */
float SoundSamples::getSampleRate() const {
    return this->sample_rate;
}

/**
 * getter function for no. of samples
 * @return
 */
int SoundSamples::getLength() const {
    return SoundSamples::length;
}

/**
 * getter function for samples array
 * @return
 */
float *SoundSamples::getsamples() const {
    return samples;
}
