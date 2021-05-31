#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <random>

//linux
#include <GL/gl.h> 
#include <GL/glu.h>

//mac
//#include <OpenGl/gl.h>
//#include <OpenGl/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/read.h"

#include "../include/geometry.h"
#include "../include/quadtree.h"
#include "../include/draw.h"
#include "../include/calcul.h"
#include "../include/init.h"

/* Dimensions initiales et titre de la fenetre */
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char WINDOW_TITLE[] = "Projet";

/* Espace fenetre virtuelle */
static int currentWidth;
static int currentHeight;

static int validateMenu = 0;

// angle de camera 
static float phi = 0.;
static float teta = 0.; 

/*Soleil tournant ou non*/
int switchSun = 0;

/*Couleur Skybox*/
ColorRGB couleurCiel = createColor(0.5, 0.5, 0.9);

/*Mode filaire*/
int switchWireframe = 0;

//gluPerspective et frustum culling

float angleHorizontal; 

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;



void onWindowResized(unsigned int width, unsigned int height, Camera camera, Timac *timac)
{ 
    float aspectRatio = (float) width / (float) height;
    currentHeight = height;
    currentWidth = width;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    
    float angleViewRad = (timac->fov/180)*M_PI;
    angleHorizontal = 2.0 * atan(tan(angleViewRad * 0.5) * aspectRatio); 
    angleHorizontal = (angleHorizontal/M_PI)*180;

   gluPerspective(timac->fov, aspectRatio, timac->Znear, timac->Zfar);
   orienteCamera(camera);
}




int main(int argc, char** argv) 
{
    //Lecture des fichier .timac et .pgm
    std::string nameHeightMap;
    int tmp, width, height, grayLvl;
    Timac timac=createTimac();
    string nomFichier= "fichier.timac";
    tmp=lectureFichier(&timac, nomFichier);
    if (!tmp){
        cerr << "impossible de lire le fichier .timac" << endl;
        return EXIT_FAILURE;
    }
    
    int *map=lecturePGM(timac.nameHeightMap, &width, &height, &grayLvl);
    if(map==NULL){
        cerr << "impossible de lire le fichier .pgm" << endl;
        return EXIT_FAILURE;
    };

    float grayLvlRatio= timac.Zmax-timac.Zmin;

    //Creation du quadtree
    Point3D pointA = createPoint(0.,0.,0.);
    Point3D pointB = createPoint(0., (float)(width-1), 0.);
    Point3D pointC = createPoint((float)(height-1), (float)(width-1), 0.);
    Point3D pointD = createPoint((float)(height-1),0.,0.);
    Node* quadtree = Node::createQuadTree(pointA,pointB,pointC,pointD,map,width, height, 0, (float)grayLvl, &timac);


    //* Initialisation de la SDL */
    SDL_GLContext context;  
    SDL_Window* window =initSDL(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, &context);
    
    // Texture
    GLuint tabTextureId[7];
    char* name[7]={"assets/grass_5.jpg","assets/grass_4.jpg", "assets/grass_3.jpg", "assets/grass_2.jpg","assets/grass_1.jpg", 
                    "assets/skybox_top.jpg", "assets/menu.jpg"};
    for(int i = 0; i<7; i++){
        tabTextureId[i] = generateTextureJpg(name[i]);
    }

    //Arbres
    GLuint arbre;
    arbre= generateTexturePng("./assets/tree.png");

    //Creation de la copie de la map en pixel
    float* mapCopy = (float*) malloc(sizeof(float)*(height*width));

    //Camera
    Flags cameraFlags= createFlags();
    Camera camera;
    camera.posCam = createPoint(1.,1., 0.);// pk pas 0 ???
    camera.posCam = createPoint(1., 1., 0.); 
    camera.viseCam = createPoint(0.,0.,0.);
    camera.up = createPoint(0.,0.,1.);

    //Soleil
    float angleSoleil = M_PI/4;

    // glu
    onWindowResized(WINDOW_WIDTH, WINDOW_HEIGHT, camera, &timac);

    /* Boucle principale */
    int loop = 1;
    while(loop) 
    {
        /* Recuperation du temps au debut de la boucle */
            Uint32 startTime = SDL_GetTicks();

        // Soit on dessine le menu, soit toute la map
        if(validateMenu == 1){
            Vector3D rayonSoleil = createPoint(100.*cos(M_PI/4),0.,100.*sin(M_PI/4));
            ColorRGB couleurSoleil = createColor(2.,2.,2.);
            Light soleil = createSun(rayonSoleil, couleurSoleil);
            // On dessine devant tous les arbres etc...
            glDisable(GL_DEPTH_TEST); // Enables Depth Testing
            drawMenu(tabTextureId[6], camera, phi, teta, soleil);
        }else{  
            // Gestion Soleil
            if (switchSun == 1) {
                angleSoleil += (2*M_PI)/500;
            }
            Vector3D rayonSoleil = createPoint(100.*cos(angleSoleil),0.,100.*sin(angleSoleil));
            ColorRGB couleurSoleil = createColor(2.,2.,2.);
            Light soleil = createSun(rayonSoleil, couleurSoleil);

            // condition initiales de la boucle
            glClear(GL_COLOR_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);
            glMatrixMode(GL_MODELVIEW);

            // Dessin de la skybox
            glPushMatrix();
            glDepthMask(GL_FALSE);
            glTranslatef(camera.posCam.x, camera.posCam.y, camera.posCam.z);
            drawCenteredBox(5., couleurCiel, tabTextureId);
            glDepthMask(GL_TRUE);
            glPopMatrix();

            //Prise en compte de la profondeur
            glClearDepth(1.f); // Depth Buffer Setup
            glEnable(GL_DEPTH_TEST); // Enables Depth Testing
            glDepthFunc(GL_LEQUAL); 
            glAlphaFunc(GL_GREATER, 0.01);

            //Prise en compte de la transparence
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            //Dessin de l'origine et de la map
            drawOrigin();       
            glPushMatrix();
            
            for(int i=0; i<(height*width); i++){
                mapCopy[i] = (float) map[i];
            }
            if (switchWireframe == 0) {
                couleurCiel = illuminationLambert(createPoint(0.5,0.,0.), createPoint(0.,0.,0.), createPoint(0.,0.5,0.), soleil);
                drawQuadTreeLOD(quadtree, soleil, tabTextureId, camera, mapCopy, width,height, grayLvl, timac.Zfar, angleHorizontal, grayLvlRatio, &timac);
            } else {
                couleurCiel = createColor(0., 0., 0.1);
                drawQuadTreeLinesLOD(quadtree,camera, mapCopy, width, height, timac.Zfar, angleHorizontal, grayLvl, &timac);
            }
            glPopMatrix();

            //Dessin des arbres
            std::default_random_engine generator;
            std::uniform_real_distribution<float> distribution(-(timac.Xsize)/2,(timac.Xsize)/2);
            if (switchWireframe == 0) {
                    for (int i = 0; i <(timac.Xsize)*2.; i++) {
                    glPushMatrix();
                    srand(4*i);
                    float x=distribution(generator);
                    std::uniform_real_distribution<float> distribution(-(timac.Ysize)/2,(timac.Ysize)/2);
                    float y= distribution(generator);
                    float z = computeZ(x, y, mapCopy, width, height, grayLvl, &timac)-0.1;
                    Vector3D distance=createVector(abs(camera.posCam.x - x), abs(camera.posCam.y - y), 0);
                    //On ne dessine les arbres que s'ils sont assez proche de nous
                    if(sqrt(pow(distance.x,2.)+pow(distance.y,2.))<=(0.2*timac.Zfar)){
                        glTranslatef(x, y, z);
                        drawBillboard(phi, arbre , createPoint(0.,3.,3.), soleil);
                    }
                    glPopMatrix();
                }
            }

            
            
            // Gestion caméra
            if(cameraFlags.flagFPS==1) { // on désactive les mouvements vers le haut et bas
                cameraFlags.flagCamUp = 0; 
                cameraFlags.flagCamDown = 0;
            }
            camera = moveCamera(camera, &cameraFlags, &teta, &phi, &timac);
            if(cameraFlags.flagFPS==1) {
                camera.posCam.z = computeZ(camera.posCam.x, camera.posCam.y, mapCopy, width, height, grayLvl, &timac)+1.;
            }

        }

        //On gere la redimension de la fenetre
        onWindowResized(currentWidth, currentHeight, camera, &timac);
         
        /* Echange du front et du back buffer : mise a jour de la fenetre */
        SDL_GL_SwapWindow(window);
            
        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) 
        {
            /* L'utilisateur ferme la fenetre : */
            if(e.type == SDL_QUIT) 
            {
                loop = 0;
                break;
            }
        
            if(	e.type == SDL_KEYDOWN 
                && (e.key.keysym.sym == SDLK_ESCAPE))
            {
                loop = 0; 
                break;
            }
            
            switch(e.type) 
            {
                case SDL_WINDOWEVENT:
                    switch (e.window.event) 
                    {
                        /* Redimensionnement fenetre */
                        case SDL_WINDOWEVENT_RESIZED:
                            onWindowResized(e.window.data1, e.window.data2, camera, &timac);                
                            break;

                        default:
                            break; 
                    }
                    break;

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    //printf("clic en (%d, %d)\n", e.button.x, e.button.y);
                    break;
                
                /* Touche clavier */
                case SDL_KEYDOWN:
                    //printf("touche pressee (code = %d)\n", e.key.keysym.sym);
                    if (e.key.keysym.sym == 104 && e.key.repeat == 0) { //H (haut)
                        cameraFlags.flagCamUp = 1;
                        cameraFlags.flagCamDown = 0;
                    }
                    if (e.key.keysym.sym == 98 && e.key.repeat == 0) { //B(bas)
                        cameraFlags.flagCamDown = 1;
                        cameraFlags.flagCamUp = 0;
                    }
                    if (e.key.keysym.sym == 1073741904 && e.key.repeat == 0) { // Flèche gauche  
                        cameraFlags.flagCamLeft = 1;
                        cameraFlags.flagCamRight = 0;
                    }
                    if (e.key.keysym.sym == 1073741903 && e.key.repeat == 0) { // Flèche droite 
                        cameraFlags.flagCamRight = 1;
                        cameraFlags.flagCamLeft = 0;
                    }
                    if (e.key.keysym.sym == 1073741906 && e.key.repeat == 0) { // Flèche haut (devant)  
                        cameraFlags.flagCamForward = 1;
                        cameraFlags.flagCamBackward = 0;
                    }
                    if (e.key.keysym.sym == 1073741905 && e.key.repeat == 0) { // Flèche bas (derrière) 
                        cameraFlags.flagCamBackward = 1;
                        cameraFlags.flagCamForward = 0;
                    }
                    if (e.key.keysym.sym == 122 && e.key.repeat == 0) { // Z (tilt up) 
                        cameraFlags.flagCamTiltUp= 1;
                        cameraFlags.flagCamTiltDown = 0;
                    }
                    if (e.key.keysym.sym == 115 && e.key.repeat == 0) { // S (tilt down) 
                        cameraFlags.flagCamTiltDown = 1;
                        cameraFlags.flagCamTiltUp = 0;
                    }
                    if (e.key.keysym.sym == 113 && e.key.repeat == 0) { // Q (pan left) 
                        cameraFlags.flagCamPanLeft = 1;
                        cameraFlags.flagCamPanRight = 0;
                    }
                    if (e.key.keysym.sym == 100 && e.key.repeat == 0) { // D (pan right) 
                        cameraFlags.flagCamPanRight = 1;
                        cameraFlags.flagCamPanLeft = 0;
                    }
                    if (e.key.keysym.sym == 108) { // l (light, soleil qui tourne)
                        switchSun = 1-switchSun;
                    }
                    if (e.key.keysym.sym == 102) { // f (mode filaire)
                        switchWireframe = 1-switchWireframe;
                    }
                    if (e.key.keysym.sym == 99 && (abs(camera.posCam.x)<=(timac.Xsize)/2. && abs(camera.posCam.y)<=(timac.Ysize)/2.)) { // c camera (mode FPS)
                        cameraFlags.flagFPS = 1 - cameraFlags.flagFPS;
                    }
                    if(e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {  // appuyer sur entrer 
                        if(validateMenu == 1){
                            validateMenu = 0;
                        }
                        else{
                            validateMenu = 1;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (e.key.keysym.sym == 104) { //H (haut)
                        cameraFlags.flagCamUp = 0;
                    }
                    if (e.key.keysym.sym == 98) { //B(bas)
                        cameraFlags.flagCamDown = 0;
                    }
                    if (e.key.keysym.sym == 1073741904) { // Flèche gauche  
                        cameraFlags.flagCamLeft = 0;
                    }
                    if (e.key.keysym.sym == 1073741903) { // Flèche droite 
                        cameraFlags.flagCamRight = 0;
                    }
                    if (e.key.keysym.sym == 1073741906) { // Flèche haut (devant) 
                        cameraFlags.flagCamForward = 0;
                    }
                    if (e.key.keysym.sym == 1073741905) { // Flèche bas (derrière)
                        cameraFlags.flagCamBackward = 0;
                    }
                    if (e.key.keysym.sym == 122) { // Z (tilt up) 
                        cameraFlags.flagCamTiltUp= 0;
                    }
                    if (e.key.keysym.sym == 115) { // S (tilt down) 
                        cameraFlags.flagCamTiltDown = 0;
                    }
                    if (e.key.keysym.sym == 113) { // Q (pan left) 
                        cameraFlags.flagCamPanLeft = 0;
                    }
                    if (e.key.keysym.sym == 100) { // D (pan right) 
                        cameraFlags.flagCamPanRight = 0;
                    }
                    break;
                    
                default:
                    break;
            }
        }

        /* Calcul du temps ecoule */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        /* Si trop peu de temps s'est ecoule, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) 
        {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    /* Liberation des ressources associees a la SDL */ 

    for(int i=0; i<7; i++){
        glDeleteTextures(1, &tabTextureId[i]);
    }
    free(mapCopy);
    free(map);


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
