#ifndef CALCUL_H
#define CALCUL_H

#include "geometry.h"
#include "quadtree.h"
#include "read.h"
// Met a jour la coordonnée Z d'un point du quadTree
void updateZ (Node* quadtree, float *map, int mapWidth, int mapHeight,float grayLvl);

//retourne l'indice du tableau contenant la plus petite valeur
int getSmallerI (float* array, int arraySize);

//Retourne le max entre x et y 
float max(float x, float y);

//Retourne le min entre x et y 
float min(float x, float y);

//regarde si le point x,y est dans le terrain. Si oui renvoie le Z, sinon renvoie 0
float computeZ (float x, float y, float* map, int mapWidth, int mapHeight,float grayLvl, Timac *timac);

//Convertit les coordonnees en pixels d'un point en coordonnees geometriques
Point3D transfoIndiceToGeometry(Point3D point, int mapWidth, int mapHeight, Timac *timac);

//Transforme des coordonnées geometriques en indices dans le tableau
float transfoXGeometryToIndice(float x, int mapWidth, int mapHeight, Timac *timac);
float transfoYGeometryToIndice(float y, int mapWidth, int mapHeight, Timac *timac);

//Prend un nombre en pixel le convertit en donnee geometrique
float transfoZIndiceToGeometry(float z, int mapWidth, int mapHeight, Timac *timac, int grayLvl);

//Recupere la coordonnee Z associee a un point dans l'espace et normalise sa valeur
void getZ(Point3D *point, int *map, int mapWidth, float grayLvl);

// retourne x s'il est entre 0 et 1, 0 si inferieur et 1 si superieur
float clampZeroOne(float x);

//Retourne le determinant d'une matrice 2x2
float determinantMatrixTwoTwo(float a, float b, float c, float d);

#endif /*CALCUL_H*/