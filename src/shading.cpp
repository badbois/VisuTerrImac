#include "../include/shading.h"

Material createMaterial(ColorRGB diffuse, ColorRGB speculaire, float shininess) {
    Material newMaterial;
    newMaterial.diffuse = diffuse;
    newMaterial.speculaire = speculaire;
    newMaterial.shininess = shininess;
    return newMaterial;
}