#include "../include/quadtree.h" 
#include "../include/geometry.h"
#include <stdlib.h>

void Node::initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD) {
    this->pointA = pointA;
    this->pointB = pointB;
    this->pointC = pointC;
    this->pointD = pointD;
    this->topLeft = nullptr;
    this->topRight = nullptr;
    this->botLeft = nullptr;
    this->botRight = nullptr;
};

/*float Node::get_value() {
    return this->value;
};*/

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

Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD) {
    Node* newNode = new Node;
    newNode->initNode(pointA, pointB, pointC, pointD);
    return newNode;
}

void getZ(Point3D *point, int *map, int mapWidth){
    int i=point->y * mapWidth + point->x;
    point->z=map[i];
}

Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth){ //PAS FINI
    
    //recuperation des coord Z des points
    getZ(&pointA, map, mapWidth);
    getZ(&pointB, map, mapWidth);
    getZ(&pointC, map, mapWidth);
    getZ(&pointD, map, mapWidth);

    //creation du noeux
    Node *newNode=createNode(pointA, pointB, pointC, pointD);

    if(){//l'espace est encore divisible par 4????

        //creation des points pour les enfants
        Point3D pointAB=createPoint((pointA.x+pointB.x)/2, (pointA.y+pointB.y)/2, 0);
        Point3D pointBC=createPoint((pointB.x+pointC.x)/2, (pointB.y+pointC.y)/2, 0);
        Point3D pointCD=createPoint((pointC.x+pointD.x)/2, (pointC.y+pointD.y)/2, 0);
        Point3D pointDA=createPoint((pointD.x+pointA.x)/2, (pointD.y+pointA.y)/2, 0);
        Point3D pointCentre=createPoint((pointA.x+pointB.x)/2, (pointC.y+pointD.y)/2, 0);

        //Creation des 4 enfants 
        newNode->topLeft = createTree(pointA, pointAB, pointCentre, pointDA, map, mapWidth);
        newNode->topRight = createTree(pointAB, pointB, pointBC, pointCentre, map, mapWidth);
        newNode->botLeft = createTree(pointCentre, pointBC, pointC, pointCD, map, mapWidth);
        newNode->botRight = createTree(pointDA, pointCentre, pointCD, pointD, map, mapWidth);
    }
    
}
    
