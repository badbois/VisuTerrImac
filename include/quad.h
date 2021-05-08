#ifndef QUAD_H
#define QUAD_H

#include "geometry.h"
#include "colors.h"
#include "shading.h"

typedef struct Sommet {
    Point3D position;
    Material materiau;
} Sommet;

typedef struct Noeud {
    Sommet sommet1; // haut gauche
    Sommet sommet2; // haut droite
    Sommet sommet3; // bas droite
    Sommet sommet4; // bas gauche
} Noeud;

typedef struct {
    Vector3D rayon;
    ColorRGB couleur;
} Light;

typedef struct {
    Point3D posCam;
    Point3D viseCam;
    Point3D up;
} Camera;

Sommet createSommet(float x, float y, float z, Material materiau);
Noeud createNoeud(Sommet s1, Sommet s2, Sommet s3, Sommet s4);
Light createSun (Vector3D rayon, ColorRGB couleur);

void drawTriangle(Sommet s1, Sommet s2, Sommet s3, Light Soleil, GLuint texture);
void drawTriangles(Noeud noeud, Light Soleil, GLuint texture);

// Pour calculer la normale (pour l'illumination), on a besoin du produit vectoriel
// Attention le produit vectoriel donne une base directe, y penser quand on donne les vecteurs AB et AC
// On veut que la normale pointe vers l'extérieur
Vector3D produitVectoriel(Vector3D AC, Vector3D AB);

//normale en s1
Vector3D normaleTriangle (Sommet s1, Sommet s2, Sommet s3); //tourner dans le sens horaire en partant du sommet s1


ColorRGB illuminationLambert(Sommet s1, Sommet s2, Sommet s3, Light Soleil);

void orienteCamera(Camera camera);

Camera moveCamera (Camera camera, int flagCamUp, int flagCamDown, int flagCamLeft, int flagCamRight,
                    int flagCamForward, int flagCamBackward, int flagCamTiltUp, int flagCamTiltDonw,
                    int flagCamPanLeft, int flagCamPanRight, float* teta, float* phi);

#endif /*QUAD_H*/