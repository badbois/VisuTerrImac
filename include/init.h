#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
using namespace std;

SDL_Window* initSDL(const char *WINDOW_TITLE, const unsigned int  WINDOW_WIDTH, const unsigned int WINDOW_HEIGHT, SDL_GLContext *context);


#endif /*INIT_H*/