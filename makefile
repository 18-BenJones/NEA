CC=g++
CFLAGS=-I

tetrismake: main.cpp 
	cc main.cpp -o main.exe $(pkg-config --cflags --libs sdl2)
	