#ifndef QUADTREE_H
#define QUADTREE_H
#include "../include/geometry.h"
//#include "../include/quad.h"

struct Node
{
    Point3D pointA,pointB,pointC,pointD;
    int depth;
    Node* topLeft;
    Node* topRight;
    Node* botRight;
    Node* botLeft;

    void initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth);
    //float get_value();
    Node* get_topLeft_child();
    Node* get_topRight_child();
    Node* get_botLeft_child();
    Node* get_botRight_child();
    bool isLeaf();
    void allLeaves(Node* leaves[], int* nbLeaves);
    
}; 

typedef Node QuadTree;

Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth);
Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth, int depth);

//void ajustePointsEnfants (float x1, float x2, float* X, float* correction);

#endif // QUADTREE_H
