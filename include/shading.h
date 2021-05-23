#ifndef SHADING_H
#define SHADING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//linux
// #include <GL/gl.h>
// #include <GL/glu.h>

//mac
#include <OpenGl/gl.h>
#include <OpenGl/glu.h>

#include "colors.h"
#include <iostream>

using namespace std;

typedef struct {
    ColorRGB diffuse;
    ColorRGB speculaire;
    float shininess;
} Material;

Material createMaterial(ColorRGB diffuse, ColorRGB speculaire, float shininess);

GLuint generateTexture(char* name);

#endif /*SHADING_H*/