#ifndef SHADING_H
#define SHADING_H

#include "colors.h"

typedef struct {
    ColorRGB diffuse;
    ColorRGB speculaire;
    float shininess;
} Material;

Material createMaterial(ColorRGB diffuse, ColorRGB speculaire, float shininess);


#endif /*SHADING_H*/