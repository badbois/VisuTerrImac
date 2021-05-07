#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/geometry.h"

Point3D createPoint(float x, float y, float z) {
    Point3D point; 
    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

Vector3D createVector(float x, float y, float z) {
    Vector3D vector; 
    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

Vector3D createVectorFromPoints(Point3D p1, Point3D p2) {
    Vector3D vector; 
    vector.x = p2.x - p1.x;
    vector.y = p2.y - p1.y;
    vector.z = p2.z - p1.z;

    return vector;
}

Point3D pointPlusVector(Point3D p, Vector3D v) {
    Point3D point;
    point.x = p.x + v.x;
    point.y = p.y + v.y;
    point.z = p.z + v.z;

    return point;
}

Vector3D addVectors(Vector3D v1, Vector3D v2) {
    Vector3D vector;
    vector.x = v1.x + v2.x;
    vector.y = v1.y + v2.y;
    vector.z = v1.z + v2.z;

    return vector;
}

Vector3D subVectors(Vector3D v1, Vector3D v2) {
    Vector3D vector;
    vector.x = v1.x - v2.x;
    vector.y = v1.y - v2.y;
    vector.z = v1.z - v2.z;

    return vector;
}

Vector3D multVector(Vector3D v, float a) {
    Vector3D vector;
    vector.x = v.x*a;
    vector.y = v.y*a;
    vector.z = v.z*a;

    return vector;
}

Vector3D divVector(Vector3D v, float a) {
    Vector3D vector;
    vector.x = v.x/a;
    vector.y = v.y/a;
    vector.z = v.z/a;

    return vector;
}

float dot(Vector3D a, Vector3D b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float norm(Vector3D v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector3D normalize(Vector3D v) {
    return divVector(v, norm(v));
}

void printPoint3D(Point3D p) {
    printf("(%f, %f, %f)", p.x, p.y, p.z);
};

void printVector3D(Vector3D v){
    printf("(%f, %f, %f)", v.x, v.y, v.z);
};

Vector3D reflect(Vector3D v, Vector3D n) {
    Vector3D R;
    Vector3D N = multVector(n, 2.0*dot(v,n));
    R = subVectors(v, N);
    return R;
}