#include <SDL2/SDL.h>
//mac
/*#include <OpenGl/gl.h>
#include <OpenGl/glu.h>*/

//linux
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/quad.h"

Sommet createSommet(float x, float y, float z, Material materiau) {
    Sommet newSommet;
    newSommet.position.x = x;
    newSommet.position.y = y;
    newSommet.position.z = z;
    newSommet.materiau = materiau;

    return newSommet;
}

Noeud createNoeud(Sommet s1, Sommet s2, Sommet s3, Sommet s4) {
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

void drawTriangle(Sommet s1, Sommet s2, Sommet s3, Light Soleil) {
    glBegin(GL_TRIANGLES);
        ColorRGB couleurS1 = illuminationLambert(s1, s2, s3, Soleil);
        glColor3f(couleurS1.r, couleurS1.g, couleurS1.b);
        glVertex3f(s1.position.x, s1.position.y, s1.position.z);

        ColorRGB couleurS2 = illuminationLambert(s2, s3, s1, Soleil);
        glColor3f(couleurS2.r, couleurS2.g, couleurS2.b);
        glVertex3f(s2.position.x, s2.position.y, s2.position.z);

        ColorRGB couleurS3 = illuminationLambert(s3, s1, s2, Soleil);
        glColor3f(couleurS3.r, couleurS3.g, couleurS3.b);
        glVertex3f(s3.position.x, s3.position.y, s3.position.z);
    glEnd();   
}


void drawTriangles(Noeud noeud, Light Soleil){
    drawTriangle(noeud.sommet1, noeud.sommet2, noeud.sommet3, Soleil);
    drawTriangle(noeud.sommet3, noeud.sommet4, noeud.sommet1, Soleil);
}

Vector3D produitVectoriel(Vector3D AC, Vector3D AB) {
    Vector3D result;
    result.x = AC.y*AB.z - AC.z * AB.y;
    result.y = AC.z*AB.x - AC.x*AB.z;
    result.z = AC.x*AB.y - AC.y*AB.x;

    return result;
}

Vector3D normaleTriangle (Sommet s1, Sommet s2, Sommet s3) {
    Vector3D AC = createVectorFromPoints(s1.position, s3.position);
    Vector3D AB = createVectorFromPoints(s1.position, s2.position);
    Vector3D normale = produitVectoriel(AC, AB);
    normale = normalize(normale);

    return normale;
}

ColorRGB illuminationLambert(Sommet s1, Sommet s2, Sommet s3, Light Soleil) {
    ColorRGB couleurPoint = createColor(0.,0.,0.);
    Vector3D normale = normaleTriangle (s1, s2, s3);
    float facteur = dot(normale, Soleil.rayon);
    ColorRGB produitCouleurs = multColors(s1.materiau.diffuse, Soleil.couleur);
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