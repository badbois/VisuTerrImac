#ifndef DRAW_H
#define DRAW_H

#include "quadtree.h"
#include "geometry.h"
#include "camera.h"
#include "colors.h"

//linux
#include <GL/gl.h>
#include <GL/glu.h>

//mac
//#include <OpenGl/gl.h>
//#include <OpenGl/glu.h>

// prend les coordonne du quadtree et appelle les fonction de dessin en appliquant le LOD
void drawQuadTreeLOD(Node* quadtree, Light soleil, GLuint texture[], Camera camera, float* map, int mapWidth, int mapHeight, float grayLvl, float zFar, float angleView, float grayLvlRatio, Timac *timac);

// Affichage triangles texturés
void drawTriangle(Point3D s1, Point3D s2, Point3D s3, Light Soleil, GLuint texture);

//Choix des texture des triangles selon la hauteur
void textureToDrawTriangles(Node noeud, Light Soleil, GLuint texture[], float grayLvlRatio);

//Prend les coordonne du quadtree et appelle les fonction de dessin filaire en appliquant le LOD
void drawQuadTreeLinesLOD(Node* quadtree, Camera camera, float* map, int mapWidth, int mapHeight,float zFar, float angleView, float grayLvl, Timac *timac);

//Dessine le noeud en affichage filaire
void drawTriangleLines(Point3D s1, Point3D s2, Point3D s3, float thickness);

//Appel drawTriangleLine sur les points du noeud
void drawTrianglesLines(Node noeud, float thickness);

//Dessine un Billboard 
void drawBillboard(float phi, GLuint texture, Point3D scale, Light Soleil);

//Dessine le menu 
void drawMenu(GLuint texture, Camera camera, float phi, float teta);

//Dessine l'origine
void drawOrigin();

//Dessine la Skybox
void drawCenteredBox(float length, ColorRGB couleurCiel, GLuint* textureSky);

//Prend un nom de texture en parametre et retourne la texture associee
GLuint generateTexture(char* name);



#endif /*DRAW_H*/