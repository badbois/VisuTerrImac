#include "../include/calcul.h"

// Met a jour la coordonnée Z d'un point du quadTree
void updateZ (Node* quadtree, float*map, int mapWidth, int mapHeight, float grayLvl) {
    Point3D A = quadtree->getPointAEnPixels();
    int iA = (A.x)*mapWidth+(A.y);
    quadtree->setPointAz(map[iA]/grayLvl);

    Point3D B = quadtree->getPointBEnPixels();
    int iB = (B.x)*mapWidth+(B.y);
    quadtree->setPointBz(map[iB]/grayLvl);

    Point3D C = quadtree->getPointCEnPixels();
    int iC = (C.x)*mapWidth+(C.y);
    quadtree->setPointCz(map[iC]/grayLvl);

    Point3D D = quadtree->getPointDEnPixels();
    int iD = (D.x)*mapWidth+(D.y);
    quadtree->setPointDz(map[iD]/grayLvl);
}

//retourne l'indice du tableau contenant la plus petite valeur
int getSmallerI (float* array, int arraySize){
    float petit = array[0];
    int indice = 0;

    for (int i=1; i<arraySize; i++) {
        if (array[i]<petit) {
            indice = i;
            petit = array [i];
        }
    }

    return indice;
}

//Retourne le max entre x et y 
float max(float x, float y) {
    if (x>=y) {
        return x;
    } else {
        return y;
    }
}

//Retourne le min entre x et y 
float min(float x, float y) {
    if (x<=y) {
        return x;
    } else {
        return y;
    }
}

//regarde si le point x,y est dans le terrain. Si oui renvoie le Z, sinon renvoie 0
float computeZ (float x, float y, float* map, int mapWidth, int mapHeight, float grayLvl, Timac *timac) {
    if (abs(x)>(timac->Xsize)/2. || abs(y)>(timac->Ysize)/2.) {
        return 0;
    }
    
    
    float indiceX=transfoXGeometryToIndice(x, mapWidth, mapHeight, timac);
    int i = floor(indiceX);
    int l= ceil(indiceX);

    if (i==l){
        l++;
    }

    float indiceY=transfoYGeometryToIndice(y, mapWidth, mapHeight, timac);
    int j = floor(indiceY);
    int k = ceil(indiceY);
    
    if (j==k){
        k++;
    }

    float xDecimal = indiceX-i;
    float yDecimal = indiceY-j;


    Point3D A = createPoint((i), (j), map[i*mapWidth+j]);
    Point3D B = createPoint((i), (k), map[i*mapWidth+k]);
    Point3D C = createPoint((l), (k), map[l*mapWidth+k]);
    Point3D D = createPoint((l), (j), map[l*mapWidth+j]);

    float Z;

    if (xDecimal>yDecimal) { // on est dans le triangle du bas du noeud : ACD
        Vector3D nPlan = produitVectoriel(createVectorFromPoints(D,C), createVectorFromPoints(D,A));
        float d = A.x*nPlan.x+A.y*nPlan.y+A.z*nPlan.z;
        Z = (-nPlan.x*indiceX-nPlan.y*indiceY+d)/nPlan.z;
    } else { //on est dans le triangle en haut : ABC
        Vector3D nPlan = produitVectoriel(createVectorFromPoints(B,A), createVectorFromPoints(B,C));
        float d = A.x*nPlan.x+A.y*nPlan.y+A.z*nPlan.z;
        Z = (-nPlan.x*indiceX-nPlan.y*indiceY+d)/nPlan.z;
    }

    return transfoZIndiceToGeometry(Z, mapWidth, mapHeight, timac, grayLvl); 
}

//Convertit les coordonnees en pixels d'un point en coordonnees geometriques
Point3D transfoIndiceToGeometry(Point3D point, int mapWidth, int mapHeight, Timac *timac) {
    point.x=((point.x/(float)(mapHeight-1))* timac->Xsize)-(timac->Xsize)/2;
    point.y=((point.y/(float)(mapWidth-1))* timac->Ysize)-(timac->Ysize)/2;
    point.z=timac->Zmin + (point.z)*(timac->Zmax - timac->Zmin);
    return point;
}

//Transforme des coordonnées geometriques en indices dans le tableau
float transfoXGeometryToIndice(float x, int mapWidth, int mapHeight, Timac *timac) {
    float ix= ((x + timac->Xsize/2.)/timac->Xsize) * (float)(mapHeight-1);
    return ix;
}
float transfoYGeometryToIndice(float y, int mapWidth, int mapHeight, Timac *timac) {
    float iy= ((y + timac->Ysize/2.)/timac->Ysize) * (float)(mapWidth-1);
    return iy;
}

//Prend un nombre en pixel le convertit en donnee geometrique
float transfoZIndiceToGeometry(float z, int mapWidth, int mapHeight, Timac *timac, int grayLvl) {
    float gz=timac->Zmin + (z/grayLvl)*(timac->Zmax - timac->Zmin);
    return gz;
}

//Recupere la coordonnee Z associee a un point dans l'espace et normalise sa valeur
void getZ(Point3D *point, int *map, int mapWidth, float grayLvl){
    int i=point->x * mapWidth + point->y;
    point->z= (map[i]/grayLvl);
}

// retourne x s'il est entre 0 et 1, 0 si inferieur et 1 si superieur
float clampZeroOne(float x) {
    return max(min(1,x), 0);
}

//Retourne le determinant d'une matrice 2x2
float determinantMatrixTwoTwo(float a, float b, float c, float d) {
    return a*d-b*c;
}