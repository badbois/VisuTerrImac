#ifndef QUADTREE_H
#define QUADTREE_H

#include <SDL2/SDL.h>

#include "geometry.h"
#include "read.h"
#include "camera.h"

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

    //Initialise un node
    void initNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac);
   
    //Getter en coodonnees geometriques
    Point3D getPointA();
    Point3D getPointB();
    Point3D getPointC();
    Point3D getPointD();

    //Getter en coordonnees en pixels
    Point3D getPointAEnPixels();
    Point3D getPointBEnPixels();
    Point3D getPointCEnPixels();
    Point3D getPointDEnPixels();

    //Getter des enfants
    Node* get_topLeft_child();
    Node* get_topRight_child();
    Node* get_botLeft_child();
    Node* get_botRight_child();

    //return true si le noeud est une feuille
    bool isLeaf();

    //Renvoie un tableau des toutes les feuilles
    void allLeaves(Node* leaves[], int* nbLeaves);

    //Creer tout le quadtree
    static Node* createQuadTree(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int *map, int mapWidth, int mapHeight,int depth, float grayLvl, Timac *timac);
    
}; 



typedef Node QuadTree;

Node* createNode(Point3D pointA, Point3D pointB, Point3D pointC, Point3D pointD, int depth, int mapHeight, int mapWidth, Timac *timac);

//Trie les enfant du noeud du plus proche au plus loin
void orderChildren(Node quadtree, Node** tabPointeurEnfant, Camera camera);
#endif /*QUADTREE_H*/