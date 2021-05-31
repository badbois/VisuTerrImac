#ifndef CAMERA_H
#define CAMERA_H

//linux
#include <GL/gl.h>
#include <GL/glu.h>

//mac
//#include <OpenGl/gl.h>
//#include <OpenGl/glu.h>


#include "colors.h"
#include "geometry.h"
#include "read.h"

class Node;

typedef struct {
    Point3D posCam;
    Point3D viseCam;
    Point3D up;
} Camera;

typedef struct {
    Vector3D rayon;
    ColorRGB couleur;
} Light;

typedef struct {
    int flagCamUp;
    int flagCamDown;
    int flagCamLeft;
    int flagCamRight;
    int flagCamForward;
    int flagCamBackward;
    int flagCamPanLeft;
    int flagCamPanRight;
    int flagCamTiltUp;
    int flagCamTiltDown;
    int flagFPS;
} Flags;

// Créer une Light representant le soleil
Light createSun (Vector3D rayon, ColorRGB couleur);

//Retourne la couleur calculee d'apres l'illumination par la formule de Lambert
ColorRGB illuminationLambert(Point3D s1, Point3D s2, Point3D s3, Light Soleil);

//Creer et initialise les flags de la camera
Flags createFlags();

//Defini la position ainsi que la direction de regard de la camera 
void orienteCamera(Camera camera);

//Deplacement de la camera
Camera moveCamera (Camera camera, Flags *flags, float* teta, float* phi, Timac *timac);


//Si le noeud est present dans le champs de vision de la camera retourne 1, 0 sinon
int FrustumCulling(Camera cam, float zFar, float angleView, Node node);


//retourne la plus petite distance entre un noed et la camera
float distanceFromQuad(Node quadtree, Camera camera);


#endif /*CAMERA_H*/