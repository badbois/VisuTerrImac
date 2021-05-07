#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/quad.h"


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

static float phi = 2.;
static float teta = 2.; 

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

void onWindowResized(unsigned int width, unsigned int height, Camera camera)
{ 
    float aspectRatio = width / (float) height;
    currentHeight = height;
    currentWidth = width;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    
    /*
    if( aspectRatio > 1) 
    {
        gluOrtho2D(
        -GL_VIEW_SIZE / 2. * aspectRatio, GL_VIEW_SIZE / 2. * aspectRatio, 
        -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.);
    }
    else
    {
        gluOrtho2D(
        -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
        -GL_VIEW_SIZE / 2. / aspectRatio, GL_VIEW_SIZE / 2. / aspectRatio);
    }
    */
   gluPerspective(90.0, aspectRatio, 1., 100.);
   orienteCamera(camera);
}


/* Objets cannoniques */
void drawOrigin() 
{
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);

    glBegin(GL_LINES);

    glColor3f(1., 0., 0.);
    glVertex3f( 0.0 , 0.0, 0.0);
    glVertex3f( 1.0 , 0.0, 0.0);

    glColor3f(0.5, 0., 0.);
    glVertex3f(0.0 , 0.0, 0.0);
    glVertex3f(-1.0 , 0.0, 0.0);

    glColor3f(0., 1., 0.);
    glVertex3f( 0.0 , 0.0, 0.0);
    glVertex3f( 0.0 , 1.0, 0.0);

    glColor3f(0., 0.5, 0.);
    glVertex3f( 0.0 , 0.0, 0.0);
    glVertex3f( 0.0 , -1.0, 0.0);

    glColor3f(0., 0., 1.);
    glVertex3f( 0.0 , 0.0, 0.0);
    glVertex3f( 0.0 , 0.0, 1.0);

    glColor3f(0., 0., 0.5);
    glVertex3f( 0.0 , 0.0, 0.0);
    glVertex3f( 0.0 , 0.0, -1.0);

    glEnd();

    glColor3fv(currentColor);
}

int main(int argc, char** argv) 
{
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
    camera.posCam = createPoint(1.,-3.,1.);
    camera.viseCam = createPoint(0.,0.,0.);
    camera.up = createPoint(0.,0.,1.);


    // glu
  
    onWindowResized(WINDOW_WIDTH, WINDOW_HEIGHT, camera);

    // Definition du noeud a dessiner

    ColorRGB diffuse1 = createColor(0.5, 0.5, 0.5); //Gris
    ColorRGB nulle = createColor(0., 0., 0.); //Noir
    Material materiau1 = createMaterial(diffuse1, nulle, 1.);
    Sommet s1 = createSommet(0., 1., 1., materiau1);

    ColorRGB diffuse2 = createColor(0.5, 0.5, 0.5); //Gris
    Material materiau2 = createMaterial(diffuse2, nulle, 1.);
    Sommet s2 = createSommet(1., 0., 0., materiau2);

    ColorRGB diffuse3 = createColor(0.5, 0.5, 0.5); //Gris
    Material materiau3 = createMaterial(diffuse3, nulle, 1.);
    Sommet s3 = createSommet(0., -1., 1., materiau3);

    ColorRGB diffuse4 = createColor(0.5, 0.5, 0.5); //Gris
    Material materiau4 = createMaterial(diffuse4, nulle, 1.);
    Sommet s4 = createSommet(-1., 0.,0., materiau4);

    Noeud noeud = createNoeud (s1, s2, s3, s4);

    float angleSoleil = M_PI/4;

    Vector3D normale1 = normaleTriangle (s1, s2, s3);
    normale1 = addVectors(s1.position, normale1);

     Vector3D normale2 = normaleTriangle (s2, s3, s1);
    normale2 = addVectors(s2.position, normale2);

     Vector3D normale3 = normaleTriangle (s3, s1, s2);
    normale3 = addVectors(s3.position, normale3);
  
    /* Boucle principale */
    int loop = 1;
    while(loop) 
    {
        /* Recuperation du temps au debut de la boucle */
        Uint32 startTime = SDL_GetTicks();

        // Gestion Soleil
        angleSoleil += (2*M_PI)/500;
        Point3D positionSoleil = createPoint(100.*cos(angleSoleil),0.,100.*sin(angleSoleil));
        ColorRGB couleurSoleil = createColor(1.,1.,1.);
        Light Soleil = createSun(positionSoleil, couleurSoleil);

        // Gestion caméra

        camera = moveCamera(camera, flagCamUp, flagCamDown, flagCamLeft, flagCamRight,
                            flagCamForward, flagCamBackward, flagCamTiltDown, flagCamTiltUp,
                            flagCamPanLeft, flagCamPanRight, &teta, &phi);
        onWindowResized(currentWidth, currentHeight, camera);

        /* Placer ici le code de dessin */
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);

        glClearDepth(1.0f); // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST); // Enables Depth Testing
        glDepthFunc(GL_LEQUAL); 

        drawOrigin();
        drawTriangles(noeud, Soleil);

        glBegin(GL_LINES);
        glColor3f(1.,1.,1.); //blanc
        glVertex3f(s1.position.x, s1.position.y, s1.position.z);
        glVertex3f(normale1.x, normale1.y, normale1.z);

        glVertex3f(s2.position.x, s2.position.y, s2.position.z);
        glVertex3f(normale2.x, normale2.y, normale2.z);

        glVertex3f(s3.position.x, s3.position.y, s3.position.z);
        glVertex3f(normale3.x, normale3.y, normale3.z);
        glEnd();
        
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
                            onWindowResized(e.window.data1, e.window.data2, camera);                
                            break;

                        default:
                            break; 
                    }
                    break;

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    printf("clic en (%d, %d)\n", e.button.x, e.button.y);
                    break;
                
                /* Touche clavier */
                case SDL_KEYDOWN:
                    printf("touche pressee (code = %d)\n", e.key.keysym.sym);
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

                    break;
                case SDL_KEYUP:
                    printf("touche pressee (code = %d)\n", e.key.keysym.sym);
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
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
