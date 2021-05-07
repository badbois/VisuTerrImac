#include "../include/quadtree.h"

void Node::initNode(float value) {
    this->value = value;
    this->topLeft = nullptr;
    this->topRight = nullptr;
    this->botLeft = nullptr;
    this->botRight = nullptr;
};

float Node::get_value() {
    return this->value;
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

Node* createNode(float value) {
    Node* newNode = new Node;
    newNode->initNode(value);
    return newNode;
}




    
    