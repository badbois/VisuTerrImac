#ifndef READ_H
#define READ_H

#include <iostream>

using namespace std;

typedef struct {
    std::string nameHeightMap;
    int Xsize;
    int Ysize;
    int Zmin;
    int Zmax;
    int Znear;
    int Zfar;
} Timac;

Timac createTimac();
/*la fonction lectureFichier ouvre le .timac, remplis les variable ligne après ligne
Le fichier doit contenir, dans l'ordre et avec une seul variable par ligne
Le nom du fichier HeightMap, le Xsize, le Ysize, le Zmin, le Zmax, le Znear et enfin le Zfar
exemple:
heightMap.PGM
1
2
3
4
5
6
*/
int lectureFichier(Timac *timac);

int* lecturePGM(string nameHeightMap, int *width, int *height, int *grayLvl);



#endif /* READ_H */