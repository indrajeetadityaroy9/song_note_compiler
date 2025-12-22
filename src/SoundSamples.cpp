#include <iostream>
#include "SoundSamples.h"
using namespace std;

SoundSamples::SoundSamples() {
    length = 0;
    sample_rate = 8000;
    samples = nullptr;
}

SoundSamples::SoundSamples(const float *samples, int length, float sample_rate) {
    this->length = length;
    this->sample_rate = sample_rate;
    this->samples = new float[length];

    for (int i = 0; i < length; i++) {
        this->samples[i] = samples[i];
    }
}

SoundSamples::SoundSamples(int length, float sample_rate) {
    this->length = length;
    this->sample_rate = sample_rate;
    this->samples = new float[length];

    for (int i = 0; i < length; i++) {
        this->samples[i] = 0;
    }
}

SoundSamples::SoundSamples(const SoundSamples &S) {
    length = S.length;
    sample_rate = S.sample_rate;
    samples = new float[S.length];

    for (int i = 0; i < S.length; i++) {
        samples[i] = S.samples[i];
    }
}

SoundSamples::SoundSamples(SoundSamples&& S) noexcept
    : sample_rate(S.sample_rate), length(S.length), samples(S.samples) {
    S.samples = nullptr;
    S.length = 0;
}

SoundSamples::~SoundSamples() {
    delete[] samples;
}

SoundSamples &SoundSamples::operator=(const SoundSamples &S) {
    if (this != &S) {
        delete[] samples;
        samples = new float[S.length];
        length = S.length;
        sample_rate = S.sample_rate;

        for (int i = 0; i < S.length; i++) {
            samples[i] = S.samples[i];
        }
    }
    return *this;
}

SoundSamples &SoundSamples::operator=(SoundSamples&& S) noexcept {
    if (this != &S) {
        delete[] samples;
        samples = S.samples;
        length = S.length;
        sample_rate = S.sample_rate;
        S.samples = nullptr;
        S.length = 0;
    }
    return *this;
}

SoundSamples SoundSamples::operator+(const SoundSamples &S) const {
    SoundSamples a;
    a.sample_rate = S.sample_rate;
    a.length = this->length + S.length;
    a.samples = new float[a.length];

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

float &SoundSamples::operator[](int index) const {
    if (index >= length) {
        cout << "Array index out of bound, exiting";
        exit(0);
    }
    return samples[index];
}

float SoundSamples::getSampleRate() const {
    return this->sample_rate;
}

int SoundSamples::getLength() const {
    return SoundSamples::length;
}

float *SoundSamples::getsamples() const {
    return samples;
}
