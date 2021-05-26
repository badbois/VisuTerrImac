#include "../include/init.h"

SDL_Window* initSDL(const char *WINDOW_TITLE, const unsigned int  WINDOW_WIDTH, const unsigned int WINDOW_HEIGHT, SDL_GLContext *context){

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,32); 

    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        cerr << "Erreur lors de l'intialisation de la SDL : " << SDL_GetError() << endl;
        SDL_Quit();
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
            cerr << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
            SDL_Quit();
        }
    }
    
    {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        *context = SDL_GL_CreateContext(window);
    
        if(NULL == context) 
        {
            cerr << "Erreur lors de la creation du contexte OpenGL : "<< SDL_GetError() << endl;;
            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    }  
    return window;
}