PART A functions:

explicit Wave(string  name); -> super wave constructor
SoundSamples *generateSamples(float frequency, float samplerate, float duration);
virtual float generateFunction(float time); -> function generate a sample of a wave given the time

explicit SawtoothWave(const string& waveName); -> constructor can set a name of the wave
explicit SineWave(const string& waveName); -> constructor can set a name of the wave
explicit SquareWave(const string& waveName); -> constructor can set a name of the wave
explicit TriangleWave(const string& waveName); -> constructor can set a name of the wave

SoundSamples(const float *samples, int length, float sample_rate); -> specific constructor that takes float array, no. of samples, samplerate
SoundSamples(int length, float sample_rate); -> specific constructor that takes no. of samples and samplerate
SoundSamples(const SoundSamples &S); -> copy constructor that deep copies the object

float getSampleRate() const; -> getter function for sample rate
int getLength() const; -> getter function for no. of samples
float *getsamples() const; ->getter function for samples array

SoundSamples &operator=(const SoundSamples &S); -> Overloaded = operator
float &operator[](int index) const; -> Overloaded [] operator
SoundSamples operator+(const SoundSamples &S) const; -> Overloaded + operator

