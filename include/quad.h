#ifndef QUAD_H
#define QUAD_H

#include "geometry.h"
#include "colors.h"
#include "shading.h"
#include "quadtree.h"

#include <GL/gl.h>
#include <GL/glu.h>

typedef struct Noeud {
    Point3D sommet1; // haut gauche
    Point3D sommet2; // haut droite
    Point3D sommet3; // bas droite
    Point3D sommet4; // bas gauche
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

Noeud createNoeud(Point3D s1, Point3D s2, Point3D s3, Point3D s4);
Light createSun (Vector3D rayon, ColorRGB couleur);

void drawTriangle(Point3D s1, Point3D s2, Point3D s3, Light Soleil, GLuint texture);
void drawTriangles(Node noeud, Light Soleil, GLuint texture);
void drawTree(Node* quadtree, Light soleil, GLuint texture);

// Pour calculer la normale (pour l'illumination), on a besoin du produit vectoriel
// Attention le produit vectoriel donne une base directe, y penser quand on donne les vecteurs AB et AC
// On veut que la normale pointe vers l'extérieur
Vector3D produitVectoriel(Vector3D AC, Vector3D AB);

//normale en s1
Vector3D normaleTriangle (Point3D s1, Point3D s2, Point3D s3); //tourner dans le sens horaire en partant du sommet s1

ColorRGB illuminationLambert(Point3D s1, Point3D s2, Point3D s3, Light Soleil);

void orienteCamera(Camera camera);

Camera moveCamera (Camera camera, int flagCamUp, int flagCamDown, int flagCamLeft, int flagCamRight,
                    int flagCamForward, int flagCamBackward, int flagCamTiltUp, int flagCamTiltDonw,
                    int flagCamPanLeft, int flagCamPanRight, float* teta, float* phi);

#endif /*QUAD_H*/