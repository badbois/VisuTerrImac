#ifndef QUADTREE_H
#define QUADTREE_H
#include "../include/geometry.h"
struct Node
{
    Point3D pointA,pointB,pointC,pointD;
    Node* topLeft;
    Node* topRight;
    Node* botRight;
    Node* botLeft;

    void initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD);
    //float get_value();
    Node* get_topLeft_child();
    Node* get_topRight_child();
    Node* get_botLeft_child();
    Node* get_botRight_child();
    bool isLeaf();
    void allLeaves(Node* leaves[], int* nbLeaves);
    

    //cb de noeud avant la leave? 

}; 

typedef Node QuadTree;

Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD);
Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth);

#endif // QUADTREE_H
