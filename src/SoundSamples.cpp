#include <iostream>
#include <cstring>
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
    std::memcpy(this->samples, samples, length * sizeof(float));
}

SoundSamples::SoundSamples(int length, float sample_rate) {
    this->length = length;
    this->sample_rate = sample_rate;
    this->samples = new float[length];
    std::memset(this->samples, 0, length * sizeof(float));
}

SoundSamples::SoundSamples(const SoundSamples &S) {
    length = S.length;
    sample_rate = S.sample_rate;
    samples = new float[S.length];
    std::memcpy(samples, S.samples, S.length * sizeof(float));
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
        std::memcpy(samples, S.samples, S.length * sizeof(float));
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

    std::memcpy(a.samples, this->samples, this->length * sizeof(float));
    std::memcpy(a.samples + this->length, S.samples, S.length * sizeof(float));

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
    return this->length;
}

float *SoundSamples::getsamples() const {
    return samples;
}
