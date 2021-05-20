#include <SDL2/SDL.h>
//mac
//#include <OpenGl/gl.h>
//#include <OpenGl/glu.h>

//linux
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cfloat>
#include <iostream>
using namespace std;
#include "../include/quad.h"
#include "../include/quadtree.h"
#include "../include/shading.h"

Noeud createNoeud(Point3D s1, Point3D s2, Point3D s3, Point3D s4) {
    Noeud newNoeud;
    newNoeud.sommet1 = s1;
    newNoeud.sommet2 = s2;
    newNoeud.sommet3 = s3;
    newNoeud.sommet4 = s4;

    return newNoeud;
}

Light createSun (Vector3D rayon, ColorRGB couleur) {
    Light lumiere;
    lumiere.rayon = normalize(rayon);
    lumiere.couleur = couleur;
    return lumiere;
}

// Affichage triangles texturés
void drawTriangle(Point3D s1, Point3D s2, Point3D s3, Light Soleil, GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_TRIANGLES);
        ColorRGB couleurS1 = illuminationLambert(s1, s2, s3, Soleil);
        glColor3f(couleurS1.r, couleurS1.g, couleurS1.b);
        glTexCoord2f(0.,0.);
        glVertex3f(s1.x, s1.y, s1.z); 

        ColorRGB couleurS2 = illuminationLambert(s2, s3, s1, Soleil);
        glColor3f(couleurS2.r, couleurS2.g, couleurS2.b);
        glTexCoord2f(1.,0.);
        glVertex3f(s2.x, s2.y, s2.z);

        ColorRGB couleurS3 = illuminationLambert(s3, s1, s2, Soleil);
        glColor3f(couleurS3.r, couleurS3.g, couleurS3.b);
        glTexCoord2f(1.,1.);
        glVertex3f(s3.x, s3.y, s3.z);
    glEnd();   

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void drawTriangles(Node noeud, Light Soleil, GLuint texture[]){
    Point3D centre=centerOfTriangle(noeud.pointA, noeud.pointB, noeud.pointC);
    if(centre.z>=0 && centre.z<0.2){
        drawTriangle(noeud.pointA, noeud.pointB, noeud.pointC, Soleil, texture[0]);
    }else if(centre.z>=0.2 && centre.z<0.4){
        drawTriangle(noeud.pointA, noeud.pointB, noeud.pointC, Soleil, texture[1]);
    }else if(centre.z>=0.4 && centre.z<0.6){
        drawTriangle(noeud.pointA, noeud.pointB, noeud.pointC, Soleil, texture[2]);
    }else if(centre.z>=0.6 && centre.z<0.8){
        drawTriangle(noeud.pointA, noeud.pointB, noeud.pointC, Soleil, texture[3]);
    }else if(centre.z>=0.8 && centre.z<=1){
        drawTriangle(noeud.pointA, noeud.pointB, noeud.pointC, Soleil, texture[4]);
    }

    centre=centerOfTriangle(noeud.pointC, noeud.pointD, noeud.pointA);
    if(centre.z>=0 && centre.z<0.2){
        drawTriangle(noeud.pointC, noeud.pointD, noeud.pointA, Soleil, texture[0]);
    }else if(centre.z>=0.2 && centre.z<0.4){
        drawTriangle(noeud.pointC, noeud.pointD, noeud.pointA, Soleil, texture[1]);
    }else if(centre.z>=0.4 && centre.z<0.6){
        drawTriangle(noeud.pointC, noeud.pointD, noeud.pointA, Soleil, texture[2]);
    }else if(centre.z>=0.6 && centre.z<0.8){
        drawTriangle(noeud.pointC, noeud.pointD, noeud.pointA, Soleil, texture[3]);
    }else if(centre.z>=0.8 && centre.z<=1){
        drawTriangle(noeud.pointC, noeud.pointD, noeud.pointA, Soleil, texture[4]);
    }
    
}

void drawTree(Node* quadtree, Light soleil, GLuint texture[]) {
    if (quadtree->isLeaf()) {
        drawTriangles(*quadtree, soleil, texture);
    } else {
        drawTree(quadtree->botLeft, soleil, texture);
        drawTree(quadtree->topLeft, soleil, texture);
        drawTree(quadtree->botRight, soleil, texture);
        drawTree(quadtree->topRight, soleil, texture);
    }
}

void drawTreeLOD(Node* quadtree, Light soleil, GLuint texture[], Camera camera, float* map, int mapWidth, float grayLvl, float zFar, float angleView) {
    if (FrustumCulling(camera, zFar, angleView, *quadtree) == 1) {
        if (quadtree->isLeaf()) {
            updateZ(quadtree, map, mapWidth, grayLvl);
            drawTriangles(*quadtree, soleil, texture);
        } else if (distanceFromQuad(*quadtree, camera)>(2*mapWidth/(quadtree->depth+1))) {
            updateZ(quadtree, map, mapWidth, grayLvl);
            drawTriangles(*quadtree, soleil, texture);

            //Modification de la map pour éviter les cracks
            //iA, iB, iC, iD : indice des points dans la map
            Point3D A = quadtree->pointA;
            int iA = A.x*mapWidth+A.y;
            Point3D B = quadtree->pointB;
            int iB = B.x*mapWidth+B.y;
            Point3D C = quadtree->pointC;
            int iC = C.x*mapWidth+C.y;
            Point3D D = quadtree->pointD;
            int iD = D.x*mapWidth+D.y;

            float deltaAB = (float) (iB - iA);
            float zAB = B.z - A.z;
            for (int i = iA; i<iB; i++) {
                map[i] = (A.z + (i-iA)*(zAB/deltaAB))*grayLvl;
            }
            float deltaDC = (float) (iC - iD);
            float zDC = C.z - D.z;
            for (int i = iD; i<iC; i++) {
                map[i] = (D.z + (i-iD)*(zDC/deltaDC))*grayLvl;
            }
            float deltaAD = (float) ((iD-iA)/mapWidth);
            float zAD = D.z - A.z;
            for (int i = iA; i<iD; i+= mapWidth) {
                map[i] = (A.z + ((i-iA)/((float)mapWidth)*(zAD/deltaAD)))*grayLvl;
            }
            float deltaBC = (float) ((iC-iB)/mapWidth);
            float zBC = C.z - B.z;
            for (int i = iB; i<iC; i+= mapWidth) {
                map[i] = (B.z + ((i-iB)/((float)mapWidth)*(zBC/deltaBC)))*grayLvl;
            }
        } else {
            Node** tabEnfantOrdonne = (Node**) malloc(sizeof(Node*)*4);
            orderChildren(*quadtree, tabEnfantOrdonne, camera);
            drawTreeLOD(tabEnfantOrdonne[3], soleil, texture, camera, map, mapWidth, grayLvl, zFar, angleView);
            drawTreeLOD(tabEnfantOrdonne[2], soleil, texture, camera, map, mapWidth, grayLvl, zFar, angleView);
            drawTreeLOD(tabEnfantOrdonne[1], soleil, texture, camera, map, mapWidth, grayLvl, zFar, angleView);
            drawTreeLOD(tabEnfantOrdonne[0], soleil, texture, camera, map, mapWidth, grayLvl, zFar, angleView);
        }
    }
}

void updateZ (Node* quadtree, float*map, int mapWidth, float grayLvl) {
    Point3D A = quadtree->pointA;
    int iA = A.x*mapWidth+A.y;
    quadtree->pointA.z = map[iA]/grayLvl;

    Point3D B = quadtree->pointB;
    int iB = B.x*mapWidth+B.y;
    quadtree->pointB.z = map[iB]/grayLvl;

    Point3D C = quadtree->pointC;
    int iC = C.x*mapWidth+C.y;
    quadtree->pointC.z = map[iC]/grayLvl;

    Point3D D = quadtree->pointD;
    int iD = D.x*mapWidth+D.y;
    quadtree->pointD.z = map[iD]/grayLvl;
}

// Affichage filaire
void drawTriangleLines(Point3D s1, Point3D s2, Point3D s3, float thickness) {
    glLineWidth(thickness);
    glBegin(GL_LINES);
        
        glColor4f(1.-1./thickness, 1., 1./thickness, 1./thickness);
        glVertex3f(s1.x, s1.y, s1.z); 
        glVertex3f(s2.x, s2.y, s2.z);

        glVertex3f(s2.x, s2.y, s2.z);
        glVertex3f(s3.x, s3.y, s3.z);

        glVertex3f(s3.x, s3.y, s3.z);
        glVertex3f(s1.x, s1.y, s1.z); 
    glEnd();   
}

void drawTrianglesLines(Node noeud, float thickness){
    drawTriangleLines(noeud.pointA, noeud.pointB, noeud.pointC, thickness);
    drawTriangleLines(noeud.pointC, noeud.pointD, noeud.pointA, thickness);
}

// Affichage filaire sans LOD
void drawTreeLines(Node* quadtree) {
    float thickness = norm(createVectorFromPoints(quadtree->pointA, quadtree->pointB));
    //thickness = 1+(thickness/10);
    drawTrianglesLines(*quadtree, thickness);

    if (quadtree->botLeft) {
        drawTreeLines(quadtree->botLeft);
    }
    if (quadtree->topLeft) {
        drawTreeLines(quadtree->topLeft);
    }
    if (quadtree->botRight) {
        drawTreeLines(quadtree->botRight);
    }
    if (quadtree->topRight) {
        drawTreeLines(quadtree->topRight);
    }
    
}

// Affichage filaire avec LOD
void drawTreeLinesLOD(Node* quadtree, Camera camera, float* map, int mapWidth, float grayLvl) {
    float thickness = norm(createVectorFromPoints(quadtree->pointA, quadtree->pointB));

    if (quadtree->isLeaf()) {
        updateZ(quadtree, map, mapWidth, grayLvl);
        drawTrianglesLines(*quadtree, thickness);
    } else if (distanceFromQuad(*quadtree, camera)>(2*mapWidth/(quadtree->depth+1))) {
        updateZ(quadtree, map, mapWidth, grayLvl);
        drawTrianglesLines(*quadtree, thickness);

        //Modification de la map pour éviter les cracks
        //iA, iB, iC, iD : indice des points dans la map
        Point3D A = quadtree->pointA;
        int iA = A.x*mapWidth+A.y;
        Point3D B = quadtree->pointB;
        int iB = B.x*mapWidth+B.y;
        Point3D C = quadtree->pointC;
        int iC = C.x*mapWidth+C.y;
        Point3D D = quadtree->pointD;
        int iD = D.x*mapWidth+D.y;

        float deltaAB = (float) (iB - iA);
        float zAB = B.z - A.z;
        for (int i = iA; i<iB; i++) {
            map[i] = (A.z + (i-iA)*(zAB/deltaAB))*grayLvl;
        }
        float deltaDC = (float) (iC - iD);
        float zDC = C.z - D.z;
        for (int i = iD; i<iC; i++) {
            map[i] = (D.z + (i-iD)*(zDC/deltaDC))*grayLvl;
        }
        float deltaAD = (float) ((iD-iA)/mapWidth);
        float zAD = D.z - A.z;
        for (int i = iA; i<iD; i+= mapWidth) {
            map[i] = (A.z + ((i-iA)/((float)mapWidth)*(zAD/deltaAD)))*grayLvl;
        }
        float deltaBC = (float) ((iC-iB)/mapWidth);
        float zBC = C.z - B.z;
        for (int i = iB; i<iC; i+= mapWidth) {
            map[i] = (B.z + ((i-iB)/((float)mapWidth)*(zBC/deltaBC)))*grayLvl;
        }
    } else {
        Node** tabEnfantOrdonne = (Node**) malloc(sizeof(Node*)*4);
        orderChildren(*quadtree, tabEnfantOrdonne, camera);
        drawTreeLinesLOD(tabEnfantOrdonne[3], camera, map, mapWidth, grayLvl);
        drawTreeLinesLOD(tabEnfantOrdonne[2], camera, map, mapWidth, grayLvl);
        drawTreeLinesLOD(tabEnfantOrdonne[1], camera, map, mapWidth, grayLvl);
        drawTreeLinesLOD(tabEnfantOrdonne[0], camera, map, mapWidth, grayLvl);
    }
}

Vector3D produitVectoriel(Vector3D AC, Vector3D AB) {
    Vector3D result;
    result.x = AC.y*AB.z - AC.z*AB.y;
    result.y = AC.z*AB.x - AC.x*AB.z;
    result.z = AC.x*AB.y - AC.y*AB.x;

    return result;
}

Vector3D normaleTriangle (Point3D s1, Point3D s2, Point3D s3) {
    Vector3D AC = createVectorFromPoints(s1, s3);
    Vector3D AB = createVectorFromPoints(s1, s2);
    Vector3D normale = produitVectoriel(AC, AB);
    normale = normalize(normale);

    return normale;
}

ColorRGB illuminationLambert(Point3D s1, Point3D s2, Point3D s3, Light Soleil) {
    ColorRGB couleurPoint = createColor(0.,0.,0.);
    ColorRGB blanc = createColor(1.,1.,1.);
    Vector3D normale = normaleTriangle (s1, s2, s3);
    float facteur = dot(normale, Soleil.rayon);
    ColorRGB produitCouleurs = multColors(blanc, Soleil.couleur);
    couleurPoint = addColors(couleurPoint, multColor(produitCouleurs, facteur));

    return couleurPoint;

}

void orienteCamera(Camera camera){
    gluLookAt(camera.posCam.x, camera.posCam.y, camera.posCam.z,
            camera.viseCam.x, camera.viseCam.y, camera.viseCam.z,
            camera.up.x, camera.up.y, camera.up.z); 
}

Camera moveCamera (Camera camera, int flagCamUp, int flagCamDown, int flagCamLeft, int flagCamRight,
                    int flagCamForward, int flagCamBackward, int flagCamTiltDown, int flagCamTiltUp, 
                    int flagCamPanLeft, int flagCamPanRight, float* teta, float* phi) {

    camera.posCam.z += flagCamUp * 0.01;
    camera.posCam.z -= flagCamDown * 0.01;

    Vector3D dirCiel = createVector(0.,0.,1.);
    Vector3D dirRegard = createVectorFromPoints(camera.posCam, camera.viseCam);

    Vector3D Left = produitVectoriel(dirCiel, dirRegard);
    Left = normalize(Left);
    Vector3D pasLeft = multVector(Left, (0.02*flagCamLeft));
    camera.posCam = addVectors(camera.posCam, pasLeft);
    //camera.viseCam = addVectors(camera.viseCam, pasLeft); 

    Vector3D up = produitVectoriel(dirRegard, Left);
    camera.up = up;

    Vector3D pasRight = multVector(Left, (-0.02*flagCamRight));
    camera.posCam = addVectors(camera.posCam, pasRight);
    //camera.viseCam = addVectors(camera.viseCam, pasRight); 

    Vector3D Forward = produitVectoriel(Left, dirCiel);
    Forward = normalize(Forward);
    Vector3D pasForward = multVector(Forward, (0.02*flagCamForward));
    camera.posCam = addVectors(camera.posCam, pasForward);
    //camera.viseCam = addVectors(camera.viseCam, pasForward);

    Vector3D pasBackward = multVector(Forward, (-0.02*flagCamBackward));
    camera.posCam = addVectors(camera.posCam, pasBackward);
    //camera.viseCam = addVectors(camera.viseCam, pasBackward);

    if (flagCamTiltUp == 1) {
        if(*teta < 1.5) {
            *teta += 0.015;
        }
    }
    if (flagCamTiltDown == 1) {
        if (*teta > -1.5) {
            *teta -= 0.015;
        }
    }
    if (flagCamPanLeft == 1) {
        *phi += 0.015;
    }
    if (flagCamPanRight == 1) {
        *phi -= 0.015;
    }

    Vector3D rotation = createVector(5*cos(*phi)*cos(*teta), 5*sin(*phi)*cos(*teta), 5*sin(*teta));
    camera.viseCam = addVectors(camera.posCam, normalize(rotation));

    return camera;
}


int getSmallerI (float* array, int arraySize){
    float petit = array[0];
    int indice = 0;

    for (int i=1; i<arraySize; i++) {
        if (array[i]<petit) {
            indice = i;
            petit = array [i];
        }
    }

    return indice;
}

float nodeDistance(Node quadtree, Camera camera) {
    float distance1 = norm(createVectorFromPoints(quadtree.pointA, camera.posCam));
    float distance2 = norm(createVectorFromPoints(quadtree.pointB, camera.posCam));
    float distance3 = norm(createVectorFromPoints(quadtree.pointC, camera.posCam));
    float distance4 = norm(createVectorFromPoints(quadtree.pointD, camera.posCam));

    float petit = distance1;
    if (distance2<petit) {
        petit=distance2;
    }
    if (distance3<petit) {
        petit=distance3;
    }
    if (distance4<petit) {
        petit=distance4;
    }
    return petit;

    /*float* array = (float*) malloc(sizeof(float)*4);
    array[0] = norm(createVectorFromPoints(quadtree.pointA, camera.posCam));
    array[1] = norm(createVectorFromPoints(quadtree.pointB, camera.posCam));
    array[2] = norm(createVectorFromPoints(quadtree.pointC, camera.posCam));
    array[3] = norm(createVectorFromPoints(quadtree.pointD, camera.posCam));

    return array[getSmallerI(array, 4)];*/

}

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

Point3D projectionPointPlane(Point3D cam, Point3D A, Point3D B, Point3D C) {
    Vector3D normalePlan = normaleTriangle(A, B, C);
    Vector3D planCam = createVectorFromPoints(cam, A);
    float distancePlan = dot(normalePlan, planCam);

    Vector3D projection = multVector(normalePlan, distancePlan);
    Point3D projete = addVectors(cam, projection);

    return projete;
}

Point3D projectionPointDroite(Point3D reference, Point3D A, Point3D B, Point3D C) {
    Vector3D normalePlan = normaleTriangle(A, B, C);
    Vector3D segment = createVectorFromPoints(A,B);
    Vector3D normaleSegment = normalize(produitVectoriel(normalePlan, segment));

    Vector3D SegmentRef = createVectorFromPoints(reference, A);

    float distanceDroite = dot(normaleSegment, SegmentRef);

    Vector3D projection = multVector(normaleSegment, distanceDroite);
    Point3D projete = addVectors(reference, projection);

    return projete;
}

Point3D projectionPointSegment(Point3D reference, Point3D A, Point3D B) {
    Vector3D refB = createVectorFromPoints(reference, B);
    Vector3D BA = createVectorFromPoints(B,A);

    if (dot(refB, BA)>0) {
        return B;
    } else {
        Vector3D refA = createVectorFromPoints(reference, A);
        Vector3D AB = createVectorFromPoints(A,B);
        if (dot(refA, AB)>0) {
            return A;
        } else {
            return reference;
        }
    }
}

// En utilisant la méthode des coordonnées barycentrique,
// Prend un triangle et un point P dans le même plan, 
// Trouve le point du triangle le plus proche de P.
Point3D closestPointTriangleInTheSamePlane(Point3D P, Point3D A, Point3D B, Point3D C) {
    // coordonnées barycentriques de P
    Vector3D base1 = normalize(createVectorFromPoints(P,A));
    Vector3D normalePlan = normaleTriangle(A, B, C);
    Vector3D base2 = normalize(produitVectoriel(base1, normalePlan));

    Vector3D PB = createVectorFromPoints(P,B);
    Vector3D PC = createVectorFromPoints(P,C);
    Vector3D PA = createVectorFromPoints(P,A);

    float PBx = dot(base1, PB);
    float PBy = dot(base2, PB);
    float PCx = dot(base1, PC);
    float PCy = dot(base2, PC);
    float PAx = dot(base1, PA);
    float PAy = dot(base2, PA);

    float alpha = determinantMatrixTwoTwo(PBx, PBy, PCx, PCy);
    float beta = determinantMatrixTwoTwo(PCx, PCy, PAx, PAy);
    float gamma = determinantMatrixTwoTwo(PAx, PAy, PBx, PBy);
    float somme = alpha + beta + gamma;

    alpha = alpha/somme;
    beta = beta/somme;
    gamma = gamma/somme;

    // coordonnées barycentriques du point du triangle le plus proche de P
    Vector3D BA = createVectorFromPoints(B, A);
    Vector3D CA = createVectorFromPoints(C,A);
    Vector3D CB = createVectorFromPoints(C,B);
    Vector3D AB = createVectorFromPoints(A,B);
    Vector3D BC = createVectorFromPoints(B,C);
    Vector3D AC = createVectorFromPoints(A,C);

    float betaClose, alphaClose, gammaClose;
    if (alpha >= 0 && beta < 0) {
        if (gamma < 0 && dot(PA, BA)>0) {
            betaClose = min(1, (dot(PA,BA)/dot(BA,BA)));
            gammaClose = 0;
        } else {
            betaClose = 0;
            gammaClose = clampZeroOne(dot(PA,CA)/dot(CA, CA));
        }
        alphaClose = 1-betaClose-gammaClose;
    } else if (beta >= 0 && gamma < 0) {
        if (alpha < 0 && dot(PB, CB)>0) {
            gammaClose = min(1, (dot(PB,CB)/dot(CB,CB)));
            alphaClose = 0;
        } else {
            gammaClose = 0;
            alphaClose = clampZeroOne(dot(PB,AB)/dot(AB, AB));
        }
        betaClose = 1-gammaClose-alphaClose;
    } else if (gamma >= 0 && alpha < 0) {
        if (beta < 0 && dot(PC, AC)>0) {
            alphaClose = min(1, (dot(PC,AC)/dot(AC,AC)));
            betaClose = 0;
        } else {
            alphaClose = 0;
            betaClose = clampZeroOne(dot(PC,BC)/dot(BC, BC));
        }
        gammaClose = 1-alphaClose-betaClose;
    } else {
        alphaClose = alpha;
        betaClose = beta;
        gammaClose = gamma;
    }

    // coordonnées cartésienne du point du triangle le plus proche de P
    Point3D closePoint = addVectors(multVector(A, alphaClose), multVector(B, betaClose));
    closePoint = addVectors(closePoint, multVector(C, gammaClose));

    return closePoint;
}

float max(float x, float y) {
    if (x>=y) {
        return x;
    } else {
        return y;
    }
}

float min(float x, float y) {
    if (x<=y) {
        return x;
    } else {
        return y;
    }
}

float clampZeroOne(float x) {
    return max(min(1,x), 0);
}

float determinantMatrixTwoTwo(float a, float b, float c, float d) {
    return a*d-b*c;
}

float distanceFromQuad(Node quadtree, Camera camera) {
    Point3D A = quadtree.pointA;
    Point3D B = quadtree.pointB;
    Point3D C = quadtree.pointC;
    Point3D D = quadtree.pointD;

    // test triangle 1
    Point3D projeteTest1 = projectionPointPlane(camera.posCam, A, B, C);
    Point3D plusProcheTest1 = closestPointTriangleInTheSamePlane(projeteTest1, A, B, C);
    float distance1 = norm(createVectorFromPoints(camera.posCam,plusProcheTest1));

    // test triangle 2
    Point3D projeteTest2 = projectionPointPlane(camera.posCam, C, D, A);
    Point3D plusProcheTest2 = closestPointTriangleInTheSamePlane(projeteTest2, C, D, A);
    float distance2 = norm(createVectorFromPoints(camera.posCam,plusProcheTest2));

    return min(distance1, distance2);
}
/*
int FrustumCulling(Camera cam, float zFar, float angleView, Node node) {
    Vector3D dirRegard = createVectorFromPoints(cam.posCam, cam.viseCam);
    dirRegard.z = 0.;
    dirRegard = normalize(dirRegard);
    float teta = ((angleView/2.)/180)*M_PI;
    float distancePA = zFar/cos(teta);
    Vector3D PA = multVector(dirRegard, distancePA);

    // Calcul des deux autres points du triangle de vue (en + de la position de la caméra)
    Point3D A,B;
    A.x = cos(teta)*PA.x-sin(teta)*PA.y;
    A.y = sin(teta)*PA.x+PA.y*cos(teta);
    A = addVectors(A, cam.posCam);
    A.z = 0.;
    B.x = cos(teta)*PA.x+sin(teta)*PA.y;
    B.y = -sin(teta)*PA.x+PA.y*cos(teta);
    B = addVectors(B, cam.posCam);
    B.z = 0.;

    //Calculs des vecteurs normaux aux cotés du triangle
    Vector3D nFar = multVector(dirRegard, -1.);
    Vector3D nPA = createVector(A.y, -A.x, 0.);
    Vector3D nPB = createVector(-B.y, B.x, 0.);

    //Définition des points du node en 2D
    Point3D pt1 = createPoint(node.pointA.x, node.pointA.y, 0.);
    Point3D pt2 = createPoint(node.pointB.x, node.pointB.y, 0.);
    Point3D pt3 = createPoint(node.pointC.x, node.pointC.y, 0.);
    Point3D pt4 = createPoint(node.pointD.x, node.pointD.y, 0.);

    //Tests droite 1 (les 4 points sont ils du "bon" coté ?)
    float pt1d1 = dot(createVectorFromPoints(A, pt1), nFar);
    float pt2d1 = dot(createVectorFromPoints(A, pt2), nFar);
    float pt3d1 = dot(createVectorFromPoints(A, pt3), nFar);
    float pt4d1 = dot(createVectorFromPoints(A, pt4), nFar);
        if (pt1d1 < 0 && pt2d1 < 0 && pt3d1 < 0 && pt4d1 < 0){
            return 0; // la forme n'est pas visible
        } else {
            //Tests droite 2 (les 4 points sont ils du "bon" coté ?)
            float pt1d2 = dot(createVectorFromPoints(cam.posCam, pt1), nPA);
            float pt2d2 = dot(createVectorFromPoints(cam.posCam, pt2), nPA);
            float pt3d2 = dot(createVectorFromPoints(cam.posCam, pt3), nPA);
            float pt4d2 = dot(createVectorFromPoints(cam.posCam, pt4), nPA);
                if (pt1d2 < 0 && pt2d2 < 0 && pt3d2 < 0 && pt4d2 < 0){
                    return 0; // la forme n'est pas visible
                } else {
                    //Tests droite 3 (les 4 points sont ils du "bon" coté ?)
                    float pt1d3 = dot(createVectorFromPoints(cam.posCam, pt1), nPB);
                    float pt2d3 = dot(createVectorFromPoints(cam.posCam, pt2), nPB);
                    float pt3d3 = dot(createVectorFromPoints(cam.posCam, pt3), nPB);
                    float pt4d3 = dot(createVectorFromPoints(cam.posCam, pt4), nPB);
                        if (pt1d3 < 0 && pt2d3 < 0 && pt3d3 < 0 && pt4d3 < 0){
                            return 0; // la forme n'est pas visible
                        } 
                }
            }
    // Sinon la forme est visible
    return 1;

}
*/

int FrustumCulling(Camera cam, float zFar, float angleView, Node node) {
    Vector3D dirRegard = createVectorFromPoints(cam.posCam, cam.viseCam);
    dirRegard = normalize(dirRegard);
    Vector3D chemin1 = multVector(dirRegard, zFar);
    float teta = ((angleView/2.)/180)*M_PI;
    float ecart = tan(teta)*zFar;
    Vector3D ciel = createVector(0.,0.,1.);
    Vector3D left = produitVectoriel(dirRegard, ciel);
    Vector3D chemin2 = multVector(normalize(left), ecart);
    
    // A et B sont les deux autres points du "triangle visible" en plus de la camera
    Point3D A = addVectors(cam.posCam, addVectors(chemin1, chemin2));
    Vector3D PA = createVectorFromPoints(cam.posCam, A);
    Point3D B = addVectors(cam.posCam, addVectors(chemin1, multVector(chemin2, -1.)));
    Vector3D PB = createVectorFromPoints(cam.posCam, B);
    Vector3D up = produitVectoriel(dirRegard, left);

    //Calculs des vecteurs normaux aux plans droite, gauche, loin
    Vector3D nFar = multVector(dirRegard, -1.);
    Vector3D nPA = produitVectoriel(PA, up);
    Vector3D nPB = produitVectoriel(up, PB);

    //Tests droite 1 (les 4 points sont ils du "bon" coté ?)
    float pt1d1 = dot(createVectorFromPoints(A, node.pointA), nFar);
    float pt2d1 = dot(createVectorFromPoints(A, node.pointB), nFar);
    float pt3d1 = dot(createVectorFromPoints(A, node.pointC), nFar);
    float pt4d1 = dot(createVectorFromPoints(A, node.pointD), nFar);
        if (pt1d1 < 0 && pt2d1 < 0 && pt3d1 < 0 && pt4d1 < 0){
            return 0; // la forme n'est pas visible
        } else {
            //Tests droite 2 (les 4 points sont ils du "bon" coté ?)
            float pt1d2 = dot(createVectorFromPoints(cam.posCam, node.pointA), nPA);
            float pt2d2 = dot(createVectorFromPoints(cam.posCam, node.pointB), nPA);
            float pt3d2 = dot(createVectorFromPoints(cam.posCam, node.pointC), nPA);
            float pt4d2 = dot(createVectorFromPoints(cam.posCam, node.pointD), nPA);
                if (pt1d2 < 0 && pt2d2 < 0 && pt3d2 < 0 && pt4d2 < 0){
                    return 0; // la forme n'est pas visible
                } else {
                    //Tests droite 3 (les 4 points sont ils du "bon" coté ?)
                    float pt1d3 = dot(createVectorFromPoints(cam.posCam, node.pointA), nPB);
                    float pt2d3 = dot(createVectorFromPoints(cam.posCam, node.pointB), nPB);
                    float pt3d3 = dot(createVectorFromPoints(cam.posCam, node.pointC), nPB);
                    float pt4d3 = dot(createVectorFromPoints(cam.posCam, node.pointD), nPB);
                        if (pt1d3 < 0 && pt2d3 < 0 && pt3d3 < 0 && pt4d3 < 0){
                            return 0; // la forme n'est pas visible
                        } 
                }
            }
    // Sinon la forme est visible
    return 1;

}