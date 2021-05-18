#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

#include "../include/quad.h"
#include "../include/quadtree.h"

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
float angleView = 50.0;
float farView = 100.0;

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
   gluPerspective(angleView, aspectRatio, 0.1, farView);
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

void drawCenteredBox(float length, ColorRGB couleurCiel) 
{
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);

    glBegin(GL_QUADS);
    float l = length/2;

    glColor3f(couleurCiel.r, couleurCiel.g, couleurCiel.b);
    glVertex3f(-l , l, -l);
    glVertex3f(-l , l, l);
    glVertex3f(-l , -l, l);
    glVertex3f(-l , -l, -l);

    glVertex3f(-l , l, -l);
    glVertex3f(-l , l, l);
    glVertex3f(l , l, l);
    glVertex3f(l , l, -l);

    glVertex3f(l , -l, -l);
    glVertex3f(l , -l, l);
    glVertex3f(l , l, l);
    glVertex3f(l , l, -l);

    glVertex3f(l , -l, -l);
    glVertex3f(l , -l, l);
    glVertex3f(-l , -l, l);
    glVertex3f(-l , -l, -l);

    glVertex3f(-l , -l, -l);
    glVertex3f(-l , l, -l);
    glVertex3f(l , l, -l);
    glVertex3f(l , -l, -l);

    glVertex3f(-l , -l, l);
    glVertex3f(-l , l, l);
    glVertex3f(l , l, l);
    glVertex3f(l , -l, l);
    
    glEnd();

    glColor3fv(currentColor);
}

int main(int argc, char** argv) 
{
    //Implementation et test de la lecture de fichiers
    std::string nameHeightMap;
    int test,Xsize, Ysize, Zmin, Zmax, Znear, Zfar;
    int width, height, grayLvl;
    test=lectureFichier(&nameHeightMap, &Xsize, &Ysize, &Zmin, &Zmax, &Znear, &Zfar);
    if (!test){
        return EXIT_FAILURE;
    }

    int *map=lecturePGM(nameHeightMap, &width, &height, &grayLvl);
    if(map==NULL){
        return EXIT_FAILURE;
    }
    cout <<nameHeightMap<<" "<<Xsize <<" " << Ysize <<" " << Zmin <<" " << Zmax <<" " << Znear <<" " << Zfar << endl;
    cout << width <<" "<< height<< " "<< grayLvl<< endl;
    for(int i=1; i<=(height*width); i++){
        if(i%width!=0 ){
            cout << map[i-1]<< " ";
        }else{
            cout << map[i-1]<< endl;
        }
    }

    Point3D trucA = createPoint(0.,0.,0.);
    Point3D trucB = createPoint(0., (float)(width-1), 0.);
    Point3D trucC = createPoint((float)(height-1), (float)(width-1), 0.);
    Point3D trucD = createPoint((float)(height-1),0.,0.);

    Node* quadtree = createTree(trucA,trucB,trucC,trucD,map,width,0, (float)grayLvl);
    printPoint3D(quadtree->pointA);
    printPoint3D(quadtree->pointB);
    printPoint3D(quadtree->pointC);
    printPoint3D(quadtree->pointD);

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
  
    onWindowResized(WINDOW_WIDTH, WINDOW_HEIGHT, camera);

    // Texture

    char* name[]={"assets/sandstone.jpg","assets/dirt.jpg", "assets/grass.jpg", "assets/rock.jpg","assets/snow.jpg"};
    GLuint textureId[5];

    for(int i=0; i<5; i++){
        SDL_Surface* image=IMG_Load(name[i]);
        if(!image){
        }
        glGenTextures(1, &textureId[i]);
        glBindTexture(GL_TEXTURE_2D, textureId[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);        
    }



    // Image billboard

    GLuint bill;

    SDL_Surface* imageBill = IMG_Load("./assets/tree.png");
    if (!imageBill) {
        printf("erreur \n");
    } else {
        printf("ok \n");
    } 
   
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

    Noeud noeud = createNoeud (s1, s2, s3, s4);

    float angleSoleil = M_PI/4;

    Vector3D normale1 = normaleTriangle (s1, s2, s3);
    normale1 = addVectors(s1, normale1);

     Vector3D normale2 = normaleTriangle (s2, s3, s1);
    normale2 = addVectors(s2, normale2);

     Vector3D normale3 = normaleTriangle (s3, s1, s2);
    normale3 = addVectors(s3, normale3);
  
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

        // Gestion caméra

        camera = moveCamera(camera, flagCamUp, flagCamDown, flagCamLeft, flagCamRight,
                            flagCamForward, flagCamBackward, flagCamTiltDown, flagCamTiltUp,
                            flagCamPanLeft, flagCamPanRight, &teta, &phi);
        onWindowResized(currentWidth, currentHeight, camera);

        /* Placer ici le code de dessin */
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);

        // Skybox
        glPushMatrix();
        glDepthMask(GL_FALSE);
        glTranslatef(camera.posCam.x, camera.posCam.y, camera.posCam.z);
        drawCenteredBox(5., couleurCiel);
        //glRotatef(180, 0.,0.,1.);
        //drawCenteredBox(1., couleurCiel);
        glDepthMask(GL_TRUE);
        glPopMatrix();

        //Prise en compte de la profondeur
        glClearDepth(1.0f); // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST); // Enables Depth Testing
        glDepthFunc(GL_LEQUAL); 

        //Prise en compte de la transparence
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Origine et triangles
        drawOrigin();
        //drawTriangles(noeud, Soleil, grass);
        glPushMatrix();
        //glTranslatef(-((float)width-1.)/2., -((float)height-1.)/2., 0.);
        //printf("width : %d, height: %d", width, height );
        float* mapCopy = (float*) malloc(sizeof(float)*(height*width));
        for(int i=0; i<(height*width); i++){
            mapCopy[i] = (float) map[i];
        }
        if (switchWireframe == 0) {
            couleurCiel = createColor(0.5, 0.5, 0.9);
            //drawTree(quadtree, Soleil, grass);
            drawTreeLOD(quadtree, Soleil, textureId, camera, mapCopy, width, grayLvl, farView, angleView);
        } else {
            couleurCiel = createColor(0., 0., 0.1);
            drawTreeLinesLOD(quadtree,camera, mapCopy, width, grayLvl);
        }
        glPopMatrix();
        
/*
        //Normales
        glBegin(GL_LINES);
        glColor3f(1.,1.,1.); //blanc
        glVertex3f(s1.x, s1.y, s1.z);
        glVertex3f(normale1.x, normale1.y, normale1.z);

        glVertex3f(s2.x, s2.y, s2.z);
        glVertex3f(normale2.x, normale2.y, normale2.z);

        glVertex3f(s3.x, s3.y, s3.z);
        glVertex3f(normale3.x, normale3.y, normale3.z);
        glEnd();
        */

        // test Billboard
       
       
        glPushMatrix();
            glPushMatrix();
            glRotatef(phi*(360/6.18),0.,0.,1.);
            // printf("%f \n", phi);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, bill);
            glBegin(GL_QUADS);

               // glColor3f(1.,0.,1.);

                glTexCoord2f(0.,0.);
                glVertex3f(0.,-0.5,1.);

                glTexCoord2f(1.,0.);
                glVertex3f(0.,0.5,1.);

                glTexCoord2f(1.,1.);
                glVertex3f(0.,0.5,0.);

                glTexCoord2f(0.,1.);
                glVertex3f(0.,-0.5,0.);

            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);

            glPopMatrix();
        glPopMatrix();

        
        
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
        glDeleteTextures(1, &textureId[i]);
    }
    glDeleteTextures(1, &bill);
    SDL_FreeSurface(imageBill);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
