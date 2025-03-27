# CS NEA

This repository contains the code dump for my 2025 AQA computer science Non-examination assesment (coursework), 
which is a fully-featured version of the classic arcade game tetris using C++ and the SDL2 library

# Installation:

To install the application, you must first build it for your system.

This can be done by linking your compiler of choice to the SDL2 library.

## Mac/Linux:

first install the package using apt or equivilant 
```bash
sudo apt-install sdl2 
```

then at compile time, when specifiying include files use:

 ```$(pkg-config --cflags --libs sdl2)```


