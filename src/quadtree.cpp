#include "../include/quadtree.h" 
#include "../include/geometry.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>

void Node::initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth) {
    this->pointA = pointA;
    this->pointB = pointB;
    this->pointC = pointC;
    this->pointD = pointD;
    this->depth = depth;
    this->topLeft = nullptr;
    this->topRight = nullptr;
    this->botLeft = nullptr;
    this->botRight = nullptr;
};

Node* Node::get_topLeft_child() {
    return this->topLeft;
};

Node* Node::get_topRight_child(){
    return this->topRight;
};

Node* Node::get_botLeft_child(){
    return this->botLeft;
};

Node* Node::get_botRight_child(){
    return this->botRight;
};

bool Node::isLeaf() {
    if (this->topLeft == nullptr && this->topRight == nullptr
    && this->botLeft == nullptr && this->botRight == nullptr) {
    return true;
    }
    else {
    return false;
    }
};

void Node::allLeaves(Node* leaves[], int* nbLeaves){
    if (this->isLeaf() == true) {
        leaves[*nbLeaves] = this;
        (*nbLeaves)++;
    }
    else {
        if (this->topLeft != nullptr) {
            this->topLeft->allLeaves(leaves, nbLeaves);
        }
       if (this->topRight != nullptr) {
             this->topRight->allLeaves(leaves, nbLeaves);
        }
        if (this->botLeft != nullptr) {
            this->botLeft->allLeaves(leaves, nbLeaves);
        }
        if (this->botRight != nullptr) {
            this->botRight->allLeaves(leaves, nbLeaves);
        }
     }
};

Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth) {
    Node* newNode = new Node;
    newNode->initNode(pointA, pointB, pointC, pointD, depth);
    return newNode;
}

void getZ(Point3D *point, int *map, int mapWidth, float grayLvl){
    int i=point->x * mapWidth + point->y;
    point->z= (map[i]/grayLvl);
    //point->z= map[i];
}

/*Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth){ //PAS FINI
    
    //recuperation des coord Z des points
    getZ(&pointA, map, mapWidth);
    getZ(&pointB, map, mapWidth);
    getZ(&pointC, map, mapWidth);
    getZ(&pointD, map, mapWidth);

    //creation du noeux
    Node *newNode=createNode(pointA, pointB, pointC, pointD);

    // Calculs des coordonnées des points enfants 
    // + de la "correction" : 0 si c'est un entier, + 1 sinon
    // si ce n'est pas entier on arrondit en dessous et on calcule 2 points 
    // avec et sans "correction"
    float ABx;
    float correctABx;
    ajustePointsEnfants(pointA.x, pointB.x, &ABx, &correctABx);

    float ABy;
    float correctABy;
    ajustePointsEnfants(pointA.y, pointB.y, &ABy, &correctABy);

    float BCx;
    float correctBCx;
    ajustePointsEnfants(pointB.x, pointC.x, &BCx, &correctBCx);

    float BCy;
    float correctBCy;
    ajustePointsEnfants(pointB.y, pointC.y, &BCy, &correctBCy);

    float CDx;
    float correctCDx;
    ajustePointsEnfants(pointC.x, pointD.x, &CDx, &correctCDx);

    float CDy;
    float correctCDy;
    ajustePointsEnfants(pointC.y, pointD.y, &CDy, &correctCDy);

    float DAx;
    float correctDAx;
    ajustePointsEnfants(pointD.x, pointA.x, &DAx, &correctDAx);

    float DAy;
    float correctDAy;
    ajustePointsEnfants(pointD.y, pointA.y, &DAy, &correctDAy);

//l'espace est encore divisible par 4????
    if(!((pointA.x == ABx && pointB.x == ABx+correctABx) && (pointB.y == BCy && pointC.y == BCy+correctBCy))){

        //creation des points pour les enfants
        Point3D pointAB=createPoint((pointA.x+pointB.x)/2, (pointA.y+pointB.y)/2, 0);
        Point3D pointBC=createPoint((pointB.x+pointC.x)/2, (pointB.y+pointC.y)/2, 0);
        Point3D pointCD=createPoint((pointC.x+pointD.x)/2, (pointC.y+pointD.y)/2, 0);
        Point3D pointDA=createPoint((pointD.x+pointA.x)/2, (pointD.y+pointA.y)/2, 0);
        Point3D pointCentre=createPoint((pointA.x+pointB.x)/2, (pointC.y+pointD.y)/2, 0); // DAy par exemple plutôt que CD

        //Creation des 4 enfants 
        newNode->topLeft = createTree(pointA, pointAB, pointCentre, pointDA, map, mapWidth);
        newNode->topRight = createTree(pointAB, pointB, pointBC, pointCentre, map, mapWidth);
        newNode->botLeft = createTree(pointCentre, pointBC, pointC, pointCD, map, mapWidth);
        newNode->botRight = createTree(pointDA, pointCentre, pointCD, pointD, map, mapWidth);
    }

    return newNode;
    
}

void ajustePointsEnfants (float x1, float x2, float* X, float* correction) {
    float testEntier;
    *X = (x1+x2)/2;
    *correction = 0.;
    testEntier = floor(*X);
    if (*X-testEntier != 0.) {
        *X = testEntier;
        *correction = 1.;
    }
}*/
    
/*Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth){ //PAS FINI
    
    //recuperation des coord Z des points
    getZ(&pointA, map, mapWidth);
    getZ(&pointB, map, mapWidth);
    getZ(&pointC, map, mapWidth);
    getZ(&pointD, map, mapWidth);

    //creation du noeux
    Node *newNode=createNode(pointA, pointB, pointC, pointD);

    // Calculs des coordonnées des points enfants : milieu AB et BC
    // Arrondi au dessus et en dessous (égaux si nombre entier)
    float ABx1 = floorf((pointA.x+pointB.x)/2);
    float ABx2 = ceilf((pointA.x+pointB.x)/2);

    float BCy1 = floorf((pointB.x+pointC.x)/2);
    float BCy2 = ceilf((pointB.x+pointC.x)/2);

//l'espace est encore divisible par 4????
    if(!(ABx1 == pointA.x && ABx2 == pointB.x && BCy1 == pointC.y && BCy2 == pointB.y)){

        //creation des points pour les enfants
        Point3D pointAB1=createPoint(ABx1, pointA.y, 0.);
        Point3D pointAB2=createPoint(ABx2, pointA.y, 0.);

        Point3D pointBC1=createPoint(pointB.x, BCy1, 0);
        Point3D pointBC2=createPoint(pointB.x, BCy2, 0);

        Point3D pointCD1=createPoint(ABx1, pointC.y, 0);
        Point3D pointCD2=createPoint(ABx2, pointC.y, 0);

        Point3D pointDA1=createPoint(pointD.x, BCy1, 0);
        Point3D pointDA2=createPoint(pointD.x, BCy2, 0);
    
        Point3D pointCentreTL=createPoint(ABx1, BCy2, 0); 
        Point3D pointCentreTR=createPoint(ABx2, BCy2, 0);
        Point3D pointCentreBR=createPoint(ABx2, BCy1, 0);
        Point3D pointCentreBL=createPoint(ABx1, BCy1, 0);

        //Creation des 4 enfants 
        newNode->topLeft = createTree(pointA, pointAB1, pointCentreTL, pointDA2, map, mapWidth);
        newNode->topRight = createTree(pointAB2, pointB, pointBC2, pointCentreTR, map, mapWidth);
        newNode->botRight = createTree(pointCentreBR, pointBC1, pointC, pointCD2, map, mapWidth);
        newNode->botLeft = createTree(pointDA1, pointCentreBL, pointCD1, pointD, map, mapWidth);
    }

    return newNode;
    
}*/

// Version où on arrondit au dessus

Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth, int mapHeight,int depth, float grayLvl){ //PAS FINI
    
    //recuperation des coord Z des points
    getZ(&pointA, map, mapWidth, grayLvl);
    getZ(&pointB, map, mapWidth, grayLvl);
    getZ(&pointC, map, mapWidth, grayLvl);
    getZ(&pointD, map, mapWidth, grayLvl);

    //creation du noeux
    Node *newNode=createNode(pointA, pointB, pointC, pointD, depth);
    
    // Calculs des coordonnées des points enfants : milieu AB et BC
    // Arrondi au dessus (égal si nombre entier)
    float ABy = ceilf((pointA.y+pointB.y)/2);
    float BCx = ceilf((pointB.x+pointC.x)/2);

    //l'espace est encore divisible par 4????
    if(!(ABy == pointC.y && BCx == pointC.x)){

        //creation des points pour les enfants
        Point3D pointAB=createPoint(pointA.x,ABy, 0.);
        Point3D pointBC=createPoint(BCx, pointB.y, 0);
        Point3D pointCD=createPoint(pointC.x,ABy , 0);
        Point3D pointDA=createPoint(BCx, pointD.y, 0);
        Point3D pointCentre=createPoint(BCx, ABy, 0);

        //Creation des 4 enfants 
        newNode->topLeft = createTree(pointA, pointAB, pointCentre, pointDA, map, mapWidth, mapHeight, depth+1, grayLvl);
        newNode->topRight = createTree(pointAB, pointB, pointBC, pointCentre, map, mapWidth, mapHeight, depth+1, grayLvl);
        newNode->botRight = createTree(pointCentre, pointBC, pointC, pointCD, map, mapWidth, mapHeight, depth+1, grayLvl);
        newNode->botLeft = createTree(pointDA, pointCentre, pointCD, pointD, map, mapWidth, mapHeight, depth+1, grayLvl);
    }

    newNode->pointA.x = pointA.x-mapWidth/2;
    newNode->pointB.x = pointB.x-mapWidth/2;
    newNode->pointC.x = pointC.x-mapWidth/2;
    newNode->pointD.x = pointD.x-mapWidth/2;
    
    newNode->pointA.y = pointA.y-mapHeight/2;
    newNode->pointB.y = pointB.y-mapHeight/2;
    newNode->pointC.y = pointC.y-mapHeight/2;
    newNode->pointD.y = pointD.y-mapHeight/2;

    return newNode;
}

//version centre du triangle
/*Point3D centerOfTriangle(Point3D a, Point3D b, Point3D c){ //potentiellement a deplacer
    float x=(a.x+b.x+c.x)/3;
    float y=(a.y+b.y+c.y)/3;
    float z=(a.z+b.z+c.z)/3;
    return createPoint(x,y,z);
    
}*/

//version point le plus haut du triangle
Point3D centerOfTriangle(Point3D a, Point3D b, Point3D c){ //potentiellement a deplacer
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