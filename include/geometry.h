#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Point3D, Vector3D;

// Construit le point (x, y, z)
Point3D createPoint(float x, float y, float z);

// Construit le vecteur (x, y, z)
Vector3D createVector(float x, float y, float z);

// Construit le vecteur reliant les points P1 et P2
Vector3D createVectorFromPoints(Point3D p1, Point3D p2);

// Addition des vecteurs V1 et V2
Vector3D addVectors(Vector3D v1, Vector3D v2);

// Multiplication et division d'un vecteur V par un scalaire a
Vector3D multVector(Vector3D v, float a);
Vector3D divVector(Vector3D v, float a);

// Produit scalaire des vecteurs V1 et V2
float dot(Vector3D a, Vector3D b);

// Norme d'un vecteur V
float norm(Vector3D v);

// Construit le vecteur normalisé du vecteur V
Vector3D normalize(Vector3D v);

//Affiche les coordonnes d'un point dans la console
void printPoint3D(Point3D p);

//Affiche les coordonnes d'un vecteur dans la console
void printVector3D(Vector3D v);

// Pour calculer la normale (pour l'illumination), on a besoin du produit vectoriel
// Attention le produit vectoriel donne une base directe, y penser quand on donne les vecteurs AB et AC
// On veut que la normale pointe vers l'extérieur
Vector3D produitVectoriel(Vector3D AC, Vector3D AB);

//normale en s1 : tourner dans le sens horaire en partant du sommet s1
Vector3D normaleTriangle (Point3D s1, Point3D s2, Point3D s3);

//Effectue la projection d'un point sur un plan
Point3D projectionPointPlane(Point3D cam, Point3D A, Point3D B, Point3D C);

//Retourne le point le plus haut du triangle
Point3D topOfTriangle(Point3D a, Point3D b, Point3D c);

// En utilisant la méthode des coordonnées barycentrique,
// Prend un triangle et un point P dans le même plan, 
// Trouve le point du triangle le plus proche de P.
Point3D closestPointTriangleInTheSamePlane(Point3D P, Point3D A, Point3D B, Point3D C);

#endif /*GEOMETRY_H*/