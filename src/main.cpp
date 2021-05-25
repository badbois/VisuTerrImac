#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <random>

//linux
// #include <GL/gl.h> 
// #include <GL/glu.h>

//mac
#include <OpenGl/gl.h>
#include <OpenGl/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/read.h"

#include "../include/geometry.h"
#include "../include/quadtree.h"
#include "../include/draw.h"
#include "../include/calcul.h"

/* Dimensions initiales et titre de la fenetre */
static const unsigned int WINDOW_WIDTH = 800;
static const unsigned int WINDOW_HEIGHT = 600;
static const char WINDOW_TITLE[] = "Projet";

/* Espace fenetre virtuelle */
//static const float GL_VIEW_SIZE = 8.;
static int currentWidth;
static int currentHeight;

/* Gestion mouvement camera */
static int flagCamUp = 0;
static int flagCamDown = 0;
static int flagCamLeft = 0;
static int flagCamRight = 0;
static int flagCamForward = 0;
static int flagCamBackward = 0;
static int flagCamPanLeft = 0;
static int flagCamPanRight = 0;
static int flagCamTiltUp = 0;
static int flagCamTiltDown = 0;
static int flagFPS = 1;

// phi 3. pour etre aligné sur axe 
static float phi = 3.;
static float teta = -0.1; 

/*Soleil*/
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
    float aspectRatio = width / (float) height;
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
    //Implementation et test de la lecture de fichiers
    std::string nameHeightMap;
    int test;
    Timac timac=createTimac();
    int width, height, grayLvl;
    test=lectureFichier(&timac);
    if (!test){
        return EXIT_FAILURE;
    }

    int *map=lecturePGM(timac.nameHeightMap, &width, &height, &grayLvl);
    if(map==NULL){
        return EXIT_FAILURE;
    };

    float grayLvlRatio=  timac.Zmax-timac.Zmin;
    Point3D trucA = createPoint(0.,0.,0.);
    Point3D trucB = createPoint(0., (float)(width-1), 0.);
    Point3D trucC = createPoint((float)(height-1), (float)(width-1), 0.);
    Point3D trucD = createPoint((float)(height-1),0.,0.);

    Node* quadtree = Node::createQuadTree(trucA,trucB,trucC,trucD,map,width, height, 0, (float)grayLvl, &timac);


    //* Initialisation de la SDL */
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,32); 


    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        const char* error = SDL_GetError();
        fprintf(
            stderr, 
            "Erreur lors de l'intialisation de la SDL : %s\n", error);

        SDL_Quit();
        return EXIT_FAILURE;
    }
	
    /* Ouverture d'une fenetre et creation d'un contexte OpenGL */

    SDL_Window* window;
    {
        window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if(NULL == window) 
        {
            const char* error = SDL_GetError();
            fprintf(
                stderr,
                "Erreur lors de la creation de la fenetre : %s\n", error);

            SDL_Quit();
            return EXIT_FAILURE;
        }
    }
    
    SDL_GLContext context;
    {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        context = SDL_GL_CreateContext(window);
    
        if(NULL == context) 
        {
            const char* error = SDL_GetError();
            fprintf(
                stderr,
                "Erreur lors de la creation du contexte OpenGL : %s\n", error);

            SDL_DestroyWindow(window);
            SDL_Quit();
            return EXIT_FAILURE;
        }
    }    

    //Camera

    Camera camera;
    camera.posCam = createPoint(5.,7.,1.);
    camera.viseCam = createPoint(0.,0.,0.);
    camera.up = createPoint(0.,0.,1.);

    // glu
  
    onWindowResized(WINDOW_WIDTH, WINDOW_HEIGHT, camera, &timac);

    // Texture

    GLuint tabTextureId[6];
    char* name[6]={"assets/grass_5.jpg","assets/grass_4.jpg", "assets/grass_3.jpg", "assets/grass_2.jpg","assets/grass_1.jpg", 
                    "assets/skybox_top.jpg"};
    for(int i = 0; i<6; i++){
        tabTextureId[i] = generateTexture(name[i]);
    }

    //Billboard
    SDL_Surface* imageBill = IMG_Load("./assets/tree.png");
    GLuint bill;
    glGenTextures(1, &bill);
    glBindTexture(GL_TEXTURE_2D, bill);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, imageBill->w,imageBill->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBill->pixels);
    glBindTexture(GL_TEXTURE_2D,0);

    // Definition du noeud a dessiner

    Point3D s1 = createPoint(0.,1.,1.);
    Point3D s2 = createPoint(1.,0.,0.);
    Point3D s3 = createPoint(0.,-1.,1.);
    Point3D s4 = createPoint(-1.,0.,0.);

    float angleSoleil = M_PI/4;

    Vector3D normale1 = normaleTriangle (s1, s2, s3);
    normale1 = addVectors(s1, normale1);

     Vector3D normale2 = normaleTriangle (s2, s3, s1);
    normale2 = addVectors(s2, normale2);

     Vector3D normale3 = normaleTriangle (s3, s1, s2);
    normale3 = addVectors(s3, normale3);
    
    float* mapCopy = (float*) malloc(sizeof(float)*(height*width));
    /* Boucle principale */
    int loop = 1;
    while(loop) 
    {
        /* Recuperation du temps au debut de la boucle */
        Uint32 startTime = SDL_GetTicks();

        // Gestion Soleil
        if (switchSun == 1) {
            angleSoleil += (2*M_PI)/500;
        }
        Vector3D rayonSoleil = createPoint(100.*cos(angleSoleil),0.,100.*sin(angleSoleil));
        ColorRGB couleurSoleil = createColor(2.,2.,2.);
        Light Soleil = createSun(rayonSoleil, couleurSoleil);

        /* Placer ici le code de dessin */
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);

        // Skybox
        glPushMatrix();
        glDepthMask(GL_FALSE);
        glTranslatef(camera.posCam.x, camera.posCam.y, camera.posCam.z);
        drawCenteredBox(5., couleurCiel, tabTextureId);
        glDepthMask(GL_TRUE);
        glPopMatrix();

        //Prise en compte de la profondeur
        glClearDepth(1.0f); // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST); // Enables Depth Testing
        glDepthFunc(GL_LEQUAL); 
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.01);

        //Prise en compte de la transparence
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Origine et triangles
        drawOrigin();       
        glPushMatrix();
        
        for(int i=0; i<(height*width); i++){
            mapCopy[i] = (float) map[i];
        }
        if (switchWireframe == 0) {
            couleurCiel = illuminationLambert(createPoint(0.5,0.,0.), createPoint(0.,0.,0.), createPoint(0.,0.5,0.), Soleil);
            drawQuadTreeLOD(quadtree, Soleil, tabTextureId, camera, mapCopy, width,height, grayLvl, timac.Zfar, angleHorizontal, grayLvlRatio, &timac);
        } else {
            couleurCiel = createColor(0., 0., 0.1);
            drawQuadTreeLinesLOD(quadtree,camera, mapCopy, width, height, timac.Zfar, angleHorizontal, grayLvl, &timac);
        }
        glPopMatrix();

        // test Billboard
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(-(timac.Xsize)/2,(timac.Xsize)/2);

        if (switchWireframe == 0) {
                for (int i = 0; i <(timac.Xsize)/2.; i++) {
                glPushMatrix();
                srand(4*i);
                float x=distribution(generator);
                std::uniform_real_distribution<float> distribution(-(timac.Ysize)/2,(timac.Ysize)/2);
                float y= distribution(generator);
                float z = computeZ(x, y, mapCopy, width, height, grayLvl, &timac)-0.1;
                glTranslatef(x, y, z);
                drawBillboard(phi, bill , createPoint(0.,3.,3.), Soleil);
                glPopMatrix();
            }
        }
        
        
        // Gestion caméra
        if(flagFPS==1) { // on désactive les mouvements vers le haut et bas
            flagCamUp = 0; 
            flagCamDown = 0;
        }
        camera = moveCamera(camera, flagCamUp, flagCamDown, flagCamLeft, flagCamRight,
                            flagCamForward, flagCamBackward, flagCamTiltDown, flagCamTiltUp,
                            flagCamPanLeft, flagCamPanRight, &teta, &phi);
        if(flagFPS==1) {
            camera.posCam.z = computeZ(camera.posCam.x, camera.posCam.y, mapCopy, width, height, grayLvl, &timac)+1.;
        }
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
                        flagCamUp = 1;
                        flagCamDown = 0;
                    }
                    if (e.key.keysym.sym == 98 && e.key.repeat == 0) { //B(bas)
                        flagCamDown = 1;
                        flagCamUp = 0;
                    }
                    if (e.key.keysym.sym == 1073741904 && e.key.repeat == 0) { // Flèche gauche  
                        flagCamLeft = 1;
                        flagCamRight = 0;
                    }
                    if (e.key.keysym.sym == 1073741903 && e.key.repeat == 0) { // Flèche droite 
                        flagCamRight = 1;
                        flagCamLeft = 0;
                    }
                    if (e.key.keysym.sym == 1073741906 && e.key.repeat == 0) { // Flèche haut (devant)  
                        flagCamForward = 1;
                        flagCamBackward = 0;
                    }
                    if (e.key.keysym.sym == 1073741905 && e.key.repeat == 0) { // Flèche bas (derrière) 
                        flagCamBackward = 1;
                        flagCamForward = 0;
                    }
                    if (e.key.keysym.sym == 122 && e.key.repeat == 0) { // Z (tilt up) 
                        flagCamTiltUp= 1;
                        flagCamTiltDown = 0;
                    }
                    if (e.key.keysym.sym == 115 && e.key.repeat == 0) { // S (tilt down) 
                        flagCamTiltDown = 1;
                        flagCamTiltUp = 0;
                    }
                    if (e.key.keysym.sym == 113 && e.key.repeat == 0) { // Q (pan left) 
                        flagCamPanLeft = 1;
                        flagCamPanRight = 0;
                    }
                    if (e.key.keysym.sym == 100 && e.key.repeat == 0) { // D (pan right) 
                        flagCamPanRight = 1;
                        flagCamPanLeft = 0;
                    }
                    if (e.key.keysym.sym == 108) { // l (light, soleil qui tourne)
                        switchSun = 1-switchSun;
                    }
                    if (e.key.keysym.sym == 102) { // f (mode filaire)
                        switchWireframe = 1-switchWireframe;
                    }
                    if (e.key.keysym.sym == 99) { // c camera (mode FPS)
                        flagFPS = 1-flagFPS;
                    }
                    break;
                case SDL_KEYUP:
                    if (e.key.keysym.sym == 104) { //H (haut)
                        flagCamUp = 0;
                    }
                    if (e.key.keysym.sym == 98) { //B(bas)
                        flagCamDown = 0;
                    }
                    if (e.key.keysym.sym == 1073741904) { // Flèche gauche  
                        flagCamLeft = 0;
                    }
                    if (e.key.keysym.sym == 1073741903) { // Flèche droite 
                        flagCamRight = 0;
                    }
                    if (e.key.keysym.sym == 1073741906) { // Flèche haut (devant) 
                        flagCamForward = 0;
                    }
                    if (e.key.keysym.sym == 1073741905) { // Flèche bas (derrière)
                        flagCamBackward = 0;
                    }
                    if (e.key.keysym.sym == 122) { // Z (tilt up) 
                        flagCamTiltUp= 0;
                    }
                    if (e.key.keysym.sym == 115) { // S (tilt down) 
                        flagCamTiltDown = 0;
                    }
                    if (e.key.keysym.sym == 113) { // Q (pan left) 
                        flagCamPanLeft = 0;
                    }
                    if (e.key.keysym.sym == 100) { // D (pan right) 
                        flagCamPanRight = 0;
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

    for(int i=0; i<5; i++){
        glDeleteTextures(1, &tabTextureId[i]);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
