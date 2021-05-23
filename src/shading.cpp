#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/shading.h"

Material createMaterial(ColorRGB diffuse, ColorRGB speculaire, float shininess) {
    Material newMaterial;
    newMaterial.diffuse = diffuse;
    newMaterial.speculaire = speculaire;
    newMaterial.shininess = shininess;
    return newMaterial;
}

GLuint generateTexture(char* name){
    GLuint textureId;
    
    SDL_Surface* image = IMG_Load(name);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}