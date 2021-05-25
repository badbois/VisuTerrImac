#include "../include/quadtree.h"
#include "../include/calcul.h"


//Setter
void Node::setPointAz(float z) {
    pointA.z = z;
}
void Node::setPointBz(float z) {
    pointB.z = z;
}
void Node::setPointCz(float z) {
    pointC.z = z;
}
void Node::setPointDz(float z) {
    pointD.z = z;
}

//Getter en coordonees geometriques
Point3D Node::getPointA() {
    return transfoIndiceToGeometry(pointA, mapWidth, mapHeight, timac);
}
Point3D Node::getPointB() {
    return transfoIndiceToGeometry(pointB, mapWidth, mapHeight, timac);
}
Point3D Node::getPointC() {
    return transfoIndiceToGeometry(pointC, mapWidth, mapHeight, timac);
}
Point3D Node::getPointD() {
    return transfoIndiceToGeometry(pointD, mapWidth, mapHeight, timac);
}

//Getter en coordonnees en pixel
Point3D Node::getPointAEnPixels() {
    return pointA;
}
Point3D Node::getPointBEnPixels() {
    return pointB;
}
Point3D Node::getPointCEnPixels() {
    return pointC;
}
Point3D Node::getPointDEnPixels() {
    return pointD;
}

//Getter des enfants
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

//initialisation d'un node
void Node::initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac) {
    this->pointA = pointA;
    this->pointB = pointB;
    this->pointC = pointC;
    this->pointD = pointD;
    this->depth = depth;
    this->topLeft = nullptr;
    this->topRight = nullptr;
    this->botLeft = nullptr;
    this->botRight = nullptr;
    this->mapHeight = mapHeight;
    this->mapWidth = mapWidth;
    this->timac=timac;
};

//return true si le noeud est une feuille
bool Node::isLeaf() {
    if (this->topLeft == nullptr && this->topRight == nullptr
    && this->botLeft == nullptr && this->botRight == nullptr) {
    return true;
    }
    else {
    return false;
    }
};

//Renvoie un tableau des toutes les feuilles
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

//Creer tout le quadtree
Node* Node::createQuadTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth, int mapHeight,int depth, float grayLvl, Timac *timac){
    //recuperation des coord Z des points
    getZ(&pointA, map, mapWidth, grayLvl);
    getZ(&pointB, map, mapWidth, grayLvl);
    getZ(&pointC, map, mapWidth, grayLvl);
    getZ(&pointD, map, mapWidth, grayLvl);

    //creation du noeux
    Node *newNode=createNode(pointA, pointB, pointC, pointD, depth, mapHeight, mapWidth, timac);
    
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
        newNode->topLeft = createQuadTree(pointA, pointAB, pointCentre, pointDA, map, mapWidth, mapHeight, depth+1, grayLvl, timac);
        newNode->topRight = createQuadTree(pointAB, pointB, pointBC, pointCentre, map, mapWidth, mapHeight, depth+1, grayLvl, timac);
        newNode->botRight = createQuadTree(pointCentre, pointBC, pointC, pointCD, map, mapWidth, mapHeight, depth+1, grayLvl, timac);
        newNode->botLeft = createQuadTree(pointDA, pointCentre, pointCD, pointD, map, mapWidth, mapHeight, depth+1, grayLvl, timac);
    }

    newNode->pointA.x = pointA.x;
    newNode->pointB.x = pointB.x;
    newNode->pointC.x = pointC.x;
    newNode->pointD.x = pointD.x;
    
    newNode->pointA.y = pointA.y;
    newNode->pointB.y = pointB.y;
    newNode->pointC.y = pointC.y;
    newNode->pointD.y = pointD.y;

    return newNode;
}

//Creer un noeud
Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac) {
    Node* newNode = new Node;
    newNode->initNode(pointA, pointB, pointC, pointD, depth, mapHeight, mapWidth, timac);
    return newNode;
}

//Trie les enfant du noeud du plus proche au plus loin
void orderChildren(Node quadtree, Node** tabPointeurEnfant, Camera camera) {
    float* arrayDistance = (float*) malloc(sizeof(float)*4);
    Node** childrenToSort = (Node**) malloc(sizeof(Node*)*4);

    arrayDistance[0] = distanceFromQuad(*quadtree.topLeft, camera);
    childrenToSort[0] = quadtree.topLeft;

    arrayDistance[1] = distanceFromQuad(*quadtree.topRight, camera);
    childrenToSort[1] = quadtree.topRight;

    arrayDistance[2]  = distanceFromQuad(*quadtree.botRight, camera);
    childrenToSort[2] = quadtree.botRight;

    arrayDistance[3]  = distanceFromQuad(*quadtree.botLeft, camera);
    childrenToSort[3] = quadtree.botLeft;

    int indicePetit;
    for (int i = 0; i<4; i++) {
        indicePetit = getSmallerI(arrayDistance, 4);
        tabPointeurEnfant[i] = childrenToSort[indicePetit];
        arrayDistance[indicePetit] = FLT_MAX;
    }
}
