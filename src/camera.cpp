#include "../include/camera.h"
#include "../include/geometry.h"
#include "../include/colors.h"
#include "../include/quadtree.h"

// Créer une Light representant le soleil
Light createSun (Vector3D rayon, ColorRGB couleur) {
    Light lumiere;
    lumiere.rayon = normalize(rayon);
    lumiere.couleur = couleur;
    return lumiere;
}

Flags createFlags(){
    Flags flags;
    flags.flagCamUp = 0;
    flags.flagCamDown = 0;
    flags.flagCamLeft = 0;
    flags.flagCamRight = 0;
    flags.flagCamForward = 0;
    flags.flagCamBackward = 0;
    flags.flagCamPanLeft = 0;
    flags.flagCamPanRight = 0;
    flags.flagCamTiltUp = 0;
    flags.flagCamTiltDown = 0;
    flags.flagFPS= 1;
    return flags;
}

//Retourne la couleur calculee d'apres l'illumination par la formule de Lambert
ColorRGB illuminationLambert(Point3D s1, Point3D s2, Point3D s3, Light Soleil) {
    ColorRGB couleurPoint = createColor(0.1,0.,0.3);
    ColorRGB blanc = createColor(1.,1.,1.);
    Vector3D normale = normaleTriangle (s1, s2, s3);
    float facteur = dot(normale, Soleil.rayon);
    ColorRGB produitCouleurs = multColors(blanc, Soleil.couleur);
    couleurPoint = addColors(couleurPoint, multColor(produitCouleurs, facteur));

    return couleurPoint;

}

//Defini la position ainsi que la direction de regard de la camera 
void orienteCamera(Camera camera){
    gluLookAt(camera.posCam.x, camera.posCam.y, camera.posCam.z,
            camera.viseCam.x, camera.viseCam.y, camera.viseCam.z,
            camera.up.x, camera.up.y, camera.up.z); 
}

//Deplacement de la camera
Camera moveCamera (Camera camera, Flags *flags, float* teta, float* phi, Timac *timac) {

    camera.posCam.z += flags->flagCamUp * 0.01;
    camera.posCam.z -= flags->flagCamDown * 0.01;

    Vector3D dirCiel = createVector(0.,0.,1.);
    Vector3D dirRegard = createVectorFromPoints(camera.posCam, camera.viseCam);

    Vector3D Left = produitVectoriel(dirCiel, dirRegard);
    Left = normalize(Left);
    Vector3D pasLeft = multVector(Left, (0.05*flags->flagCamLeft));
    camera.posCam = addVectors(camera.posCam, pasLeft);
    if((abs(camera.posCam.x)>=(timac->Xsize)/2. || abs(camera.posCam.y)>=(timac->Ysize)/2.) && flags->flagFPS==1){
        camera.posCam = subVectors(camera.posCam, pasLeft);
    }

    Vector3D up = produitVectoriel(dirRegard, Left);
    camera.up = up;

    Vector3D pasRight = multVector(Left, (-0.05*flags->flagCamRight));
    camera.posCam = addVectors(camera.posCam, pasRight);
    if((abs(camera.posCam.x)>=(timac->Xsize)/2. || abs(camera.posCam.y)>=(timac->Ysize)/2.)&& flags->flagFPS==1){
        camera.posCam = subVectors(camera.posCam, pasRight);
    }

    Vector3D Forward = produitVectoriel(Left, dirCiel);
    Forward = normalize(Forward);
    Vector3D pasForward = multVector(Forward, (0.05*flags->flagCamForward));
    camera.posCam = addVectors(camera.posCam, pasForward);
    if((abs(camera.posCam.x)>=(timac->Xsize)/2. || abs(camera.posCam.y)>=(timac->Ysize)/2.)&& flags->flagFPS==1){
        camera.posCam = subVectors(camera.posCam, pasForward);
    }

    Vector3D pasBackward = multVector(Forward, (-0.05*flags->flagCamBackward));
    camera.posCam = addVectors(camera.posCam, pasBackward);
    if((abs(camera.posCam.x)>=(timac->Xsize)/2. || abs(camera.posCam.y)>=(timac->Ysize)/2.)&& flags->flagFPS==1){
        camera.posCam = subVectors(camera.posCam, pasBackward);
    }


    if (flags->flagCamTiltUp == 1) {
        if(*teta < 1.5) {
            *teta += 0.015;
        }
    }
    if (flags->flagCamTiltDown == 1) {
        if (*teta > -1.5) {
            *teta -= 0.015;
        }
    }
    if (flags->flagCamPanLeft == 1) {
        *phi += 0.015;
    }
    if (flags->flagCamPanRight == 1) {
        *phi -= 0.015;
    }

    Vector3D rotation = createVector(5*cos(*phi)*cos(*teta), 5*sin(*phi)*cos(*teta), 5*sin(*teta));
    camera.viseCam = addVectors(camera.posCam, normalize(rotation));

    return camera;
}


//Si le noeud est present dans le champs de vision de la camera retourne 1, 0 sinon
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
    float pt1d1 = dot(createVectorFromPoints(A, node.getPointA()), nFar);
    float pt2d1 = dot(createVectorFromPoints(A, node.getPointB()), nFar);
    float pt3d1 = dot(createVectorFromPoints(A, node.getPointC()), nFar);
    float pt4d1 = dot(createVectorFromPoints(A, node.getPointD()), nFar);
        if (pt1d1 < 0 && pt2d1 < 0 && pt3d1 < 0 && pt4d1 < 0){
            return 0; // la forme n'est pas visible
        } else {
            //Tests droite 2 (les 4 points sont ils du "bon" coté ?)
            float pt1d2 = dot(createVectorFromPoints(cam.posCam, node.getPointA()), nPA);
            float pt2d2 = dot(createVectorFromPoints(cam.posCam, node.getPointB()), nPA);
            float pt3d2 = dot(createVectorFromPoints(cam.posCam, node.getPointC()), nPA);
            float pt4d2 = dot(createVectorFromPoints(cam.posCam, node.getPointD()), nPA);
                if (pt1d2 < 0 && pt2d2 < 0 && pt3d2 < 0 && pt4d2 < 0){
                    return 0; // la forme n'est pas visible
                } else {
                    //Tests droite 3 (les 4 points sont ils du "bon" coté ?)
                    float pt1d3 = dot(createVectorFromPoints(cam.posCam, node.getPointA()), nPB);
                    float pt2d3 = dot(createVectorFromPoints(cam.posCam, node.getPointB()), nPB);
                    float pt3d3 = dot(createVectorFromPoints(cam.posCam, node.getPointC()), nPB);
                    float pt4d3 = dot(createVectorFromPoints(cam.posCam, node.getPointD()), nPB);
                        if (pt1d3 < 0 && pt2d3 < 0 && pt3d3 < 0 && pt4d3 < 0){
                            return 0; // la forme n'est pas visible
                        } 
                }
            }
    // Sinon la forme est visible
    return 1;

}


//retourne la plus petite distance entre un noed et la camera
float distanceFromQuad(Node quadtree, Camera camera) {
    Point3D A = quadtree.getPointA();
    Point3D B = quadtree.getPointB();
    Point3D C = quadtree.getPointC();
    Point3D D = quadtree.getPointD();

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