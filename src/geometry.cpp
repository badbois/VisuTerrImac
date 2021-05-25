#include <iostream>
#include <math.h>
#include "../include/geometry.h"
#include "../include/calcul.h"

using namespace std;

// Construit le point (x, y, z)
Point3D createPoint(float x, float y, float z) {
    Point3D point; 
    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

// Construit le vecteur (x, y, z)
Vector3D createVector(float x, float y, float z) {
    Vector3D vector; 
    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

// Construit le vecteur reliant les points P1 et P2
Vector3D createVectorFromPoints(Point3D p1, Point3D p2) {
    Vector3D vector; 
    vector.x = p2.x - p1.x;
    vector.y = p2.y - p1.y;
    vector.z = p2.z - p1.z;

    return vector;
}

// Addition des vecteurs V1 et V2
Vector3D addVectors(Vector3D v1, Vector3D v2) {
    Vector3D vector;
    vector.x = v1.x + v2.x;
    vector.y = v1.y + v2.y;
    vector.z = v1.z + v2.z;

    return vector;
}

// Multiplication et division d'un vecteur V par un scalaire a
Vector3D multVector(Vector3D v, float a) {
    Vector3D vector;
    vector.x = v.x*a;
    vector.y = v.y*a;
    vector.z = v.z*a;

    return vector;
}
Vector3D divVector(Vector3D v, float a) {
    Vector3D vector;
    vector.x = v.x/a;
    vector.y = v.y/a;
    vector.z = v.z/a;

    return vector;
}

// Produit scalaire des vecteurs V1 et V2
float dot(Vector3D a, Vector3D b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Norme d'un vecteur V
float norm(Vector3D v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

// Construit le vecteur normalisé du vecteur V
Vector3D normalize(Vector3D v) {
    return divVector(v, norm(v));
}

//Affiche les coordonnes d'un point dans la console
void printPoint3D(Point3D p) {
    cout << "("<< p.x <<", " << p.y <<", "<< p.z << ")"<<endl;
};

//Affiche les coordonnes d'un vecteur dans la console
void printVector3D(Vector3D v){
    cout << "("<< v.x <<", " << v.y <<", "<< v.z << ")"<<endl;
};

// Pour calculer la normale (pour l'illumination), on a besoin du produit vectoriel
// Attention le produit vectoriel donne une base directe, y penser quand on donne les vecteurs AB et AC
// On veut que la normale pointe vers l'extérieur
Vector3D produitVectoriel(Vector3D AC, Vector3D AB) {
    Vector3D result;
    result.x = AC.y*AB.z - AC.z*AB.y;
    result.y = AC.z*AB.x - AC.x*AB.z;
    result.z = AC.x*AB.y - AC.y*AB.x;

    return result;
}

//normale en s1 : tourner dans le sens horaire en partant du sommet s1
Vector3D normaleTriangle (Point3D s1, Point3D s2, Point3D s3) {
    Vector3D AC = createVectorFromPoints(s1, s3);
    Vector3D AB = createVectorFromPoints(s1, s2);
    Vector3D normale = produitVectoriel(AC, AB);
    normale = normalize(normale);

    return normale;
}

//Effectue la projection d'un point sur un plan
Point3D projectionPointPlane(Point3D cam, Point3D A, Point3D B, Point3D C) {
    Vector3D normalePlan = normaleTriangle(A, B, C);
    Vector3D planCam = createVectorFromPoints(cam, A);
    float distancePlan = dot(normalePlan, planCam);

    Vector3D projection = multVector(normalePlan, distancePlan);
    Point3D projete = addVectors(cam, projection);

    return projete;
}

//Retourne le point le plus haut du triangle
Point3D topOfTriangle(Point3D a, Point3D b, Point3D c){ //potentiellement a deplacer
    float x=0;
    float y=0;
    float z;

    if(a.z>=b.z && a.z>=c.z){
        z=a.z;
    }else if(b.z>=a.z && b.z>=c.z){
        z=b.z;
    }else{
        z=c.z;
    }
    return createPoint(x,y,z);  
}

// En utilisant la méthode des coordonnées barycentrique,
// Prend un triangle et un point P dans le même plan, 
// Trouve le point du triangle le plus proche de P.
Point3D closestPointTriangleInTheSamePlane(Point3D P, Point3D A, Point3D B, Point3D C) {
    // coordonnées barycentriques de P
    Vector3D base1 = normalize(createVectorFromPoints(P,A));
    Vector3D normalePlan = normaleTriangle(A, B, C);
    Vector3D base2 = normalize(produitVectoriel(base1, normalePlan));

    Vector3D PB = createVectorFromPoints(P,B);
    Vector3D PC = createVectorFromPoints(P,C);
    Vector3D PA = createVectorFromPoints(P,A);

    float PBx = dot(base1, PB);
    float PBy = dot(base2, PB);
    float PCx = dot(base1, PC);
    float PCy = dot(base2, PC);
    float PAx = dot(base1, PA);
    float PAy = dot(base2, PA);

    float alpha = determinantMatrixTwoTwo(PBx, PBy, PCx, PCy);
    float beta = determinantMatrixTwoTwo(PCx, PCy, PAx, PAy);
    float gamma = determinantMatrixTwoTwo(PAx, PAy, PBx, PBy);
    float somme = alpha + beta + gamma;

    alpha = alpha/somme;
    beta = beta/somme;
    gamma = gamma/somme;

    // coordonnées barycentriques du point du triangle le plus proche de P
    Vector3D BA = createVectorFromPoints(B, A);
    Vector3D CA = createVectorFromPoints(C,A);
    Vector3D CB = createVectorFromPoints(C,B);
    Vector3D AB = createVectorFromPoints(A,B);
    Vector3D BC = createVectorFromPoints(B,C);
    Vector3D AC = createVectorFromPoints(A,C);

    float betaClose, alphaClose, gammaClose;
    if (alpha >= 0 && beta < 0) {
        if (gamma < 0 && dot(PA, BA)>0) {
            betaClose = min(1, (dot(PA,BA)/dot(BA,BA)));
            gammaClose = 0;
        } else {
            betaClose = 0;
            gammaClose = clampZeroOne(dot(PA,CA)/dot(CA, CA));
        }
        alphaClose = 1-betaClose-gammaClose;
    } else if (beta >= 0 && gamma < 0) {
        if (alpha < 0 && dot(PB, CB)>0) {
            gammaClose = min(1, (dot(PB,CB)/dot(CB,CB)));
            alphaClose = 0;
        } else {
            gammaClose = 0;
            alphaClose = clampZeroOne(dot(PB,AB)/dot(AB, AB));
        }
        betaClose = 1-gammaClose-alphaClose;
    } else if (gamma >= 0 && alpha < 0) {
        if (beta < 0 && dot(PC, AC)>0) {
            alphaClose = min(1, (dot(PC,AC)/dot(AC,AC)));
            betaClose = 0;
        } else {
            alphaClose = 0;
            betaClose = clampZeroOne(dot(PC,BC)/dot(BC, BC));
        }
        gammaClose = 1-alphaClose-betaClose;
    } else {
        alphaClose = alpha;
        betaClose = beta;
        gammaClose = gamma;
    }

    // coordonnées cartésienne du point du triangle le plus proche de P
    Point3D closePoint = addVectors(multVector(A, alphaClose), multVector(B, betaClose));
    closePoint = addVectors(closePoint, multVector(C, gammaClose));

    return closePoint;
}