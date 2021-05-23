#ifndef QUADTREE_H
#define QUADTREE_H
#include "../include/geometry.h"
#include "../include/read.h"
//#include "../include/quad.h"

struct Node
{
private:
    Point3D pointA,pointB,pointC,pointD;
public:
    void setPointAz(float z);
    void setPointBz(float z);
    void setPointCz(float z);
    void setPointDz(float z);
    int depth;
    Node* topLeft; 
    Node* topRight;
    Node* botRight;
    Node* botLeft;
    int mapWidth;
    int mapHeight;
    Timac *timac;

    void initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac);
    //float get_value();
    Point3D getPointA();
    Point3D getPointB();
    Point3D getPointC();
    Point3D getPointD();
    Point3D getPointAEnPixels();
    Point3D getPointBEnPixels();
    Point3D getPointCEnPixels();
    Point3D getPointDEnPixels();
    Node* get_topLeft_child();
    Node* get_topRight_child();
    Node* get_botLeft_child();
    Node* get_botRight_child();
    bool isLeaf();
    void allLeaves(Node* leaves[], int* nbLeaves);
    static Node* createTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth, int mapHeight,int depth, float grayLvl, Timac *timac);
    
}; 

typedef Node QuadTree;
void getZ(Point3D *point, int *map, int mapWidth, float grayLvl);
Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac);
Point3D centerOfTriangle(Point3D a, Point3D b, Point3D c); //potentiellement a deplacer/renommer
Point3D transfoIndiceToGeometry(Point3D point, int mapWidth, int mapHeight, Timac *timac);
float transfoXGeometryToIndice(float x, int mapWidth, int mapHeight, Timac *timac);
float transfoYGeometryToIndice(float y, int mapWidth, int mapHeight, Timac *timac);
float transfoZIndiceToGeometry(float z, int mapWidth, int mapHeight, Timac *timac, int grayLvl);

//void ajustePointsEnfants (float x1, float x2, float* X, float* correction);

#endif // QUADTREE_H
