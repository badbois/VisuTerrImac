#ifndef QUADTREE_H
#define QUADTREE_H

struct Node
{
    Node* topLeft;
    Node* topRight;
    Node* botRight;
    Node* botLeft;
    float value;

    void initNode(float value);
    float get_value();
    Node* get_topLeft_child();
    Node* get_topRight_child();
    Node* get_botLeft_child();
    Node* get_botRight_child();
    bool isLeaf();
    void allLeaves(Node* leaves[], int* nbLeaves);
    

    //cb de noeud avant la leave? 

}; 

typedef Node QuadTree;

Node* createNode(float value);

#endif // QUADTREE_H
