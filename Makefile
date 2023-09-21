all: genwave
parta: genwave

genwave:main.o SawtoothWave.o SineWave.o SquareWave.o TriangleWave.o SoundSamples.o soundio.o wave.o
	g++ main.o SawtoothWave.o SineWave.o SquareWave.o TriangleWave.o SoundSamples.o soundio.o wave.o -o genwave -lm

main.o:main.cpp SawtoothWave.h SineWave.h SquareWave.h SoundSamples.h wave.h soundio.h
	g++ -g -c main.cpp

SawtoothWave.o:SawtoothWave.cpp
	g++ -g -c SawtoothWave.cpp

SineWave.o:SineWave.cpp
	g++ -g -c SineWave.cpp

SquareWave.o:SquareWave.cpp
	g++ -g -c SquareWave.cpp

TriangleWave.o:TriangleWave.cpp
	g++ -g -c TriangleWave.cpp

SoundSamples.o:SoundSamples.cpp
	g++ -g -c SoundSamples.cpp

soundio.o:soundio.cpp
	g++ -g -c soundio.cpp

wave.o:wave.cpp
	g++ -g -c wave.cpp

clean:
	rm -f *.o genwave
