#ifndef READ_H
#define READ_H

#include <iostream>

using namespace std;

typedef struct {
    std::string nameHeightMap;
    float Xsize;
    float Ysize;
    float Zmin;
    float Zmax;
    float Znear;
    float Zfar;
    float fov;
} Timac;

//Creer un Timac
Timac createTimac();

/*la fonction lectureFichier ouvre le .timac, remplis les variable ligne après ligne
Le fichier doit contenir, dans l'ordre et avec une seul variable par ligne
Le nom du fichier HeightMap, le Xsize, le Ysize, le Zmin, le Zmax, le Znear, le Zfar et enfin le fov

*/
int lectureFichier(Timac *timac, string nomFichier);

/*Lecture du fichier pgm afin de recuperer le nombre de pixel de large et de haut
de la map ainsi que le niveau de gris maximal. Les valeurs recuperees sont stocke dans un tableau map*/
int* lecturePGM(string nameHeightMap, int *width, int *height, int *grayLvl);



#endif /* READ_H */