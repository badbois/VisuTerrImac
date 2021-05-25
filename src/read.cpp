#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "read.h"
using namespace std;

Timac createTimac(){
    Timac timac;
    timac.nameHeightMap="";
    timac.Xsize=0;
    timac.Ysize=0;
    timac.Zmin=0;
    timac.Zmax=0;
    timac.Zfar=0;
    timac.Znear=0;
    timac.fov=0;
    return timac;
}



int lectureFichier(Timac *timac){
    string line[8];
    ifstream monFlux("./assets/test.timac");  //Ouverture d'un fichier en lecture

    if(monFlux) //Si l'ouverture du fichier a reussi on lit chaque ligne
    {
        for(int i=0; i<8; i++){
            if(!getline(monFlux, line[i])){
                cout << "ERREUR: fichier incomplet." << endl;
                return 0;
            }
        }
        timac->nameHeightMap=line[0].c_str(); //On place chaque ligne dans une variable
        timac->Xsize=atof(line[1].c_str());
        timac->Ysize=atof(line[2].c_str());
        timac->Zmin=atof(line[3].c_str());
        timac->Zmax=atof(line[4].c_str());
        timac->Znear=atof(line[5].c_str());
        timac->Zfar=atof(line[6].c_str());
        timac->fov=atof(line[7].c_str());
        return 1;
    }
    else
    {
        cout << "ERREUR: Impossible d'ouvrir le fichier en lecture." << endl;
        return 0;
    }
}

//Lecture du pgm image.pgm qui nous a servi de test
// int* lecturePGM(string nameHeightMap, int *width, int *height, int *grayLvl){
//     string line;
//     int iterator=0;
//     ifstream monFlux("./assets/"+nameHeightMap);
//     if(monFlux){
//         //On skip la premiere ligne qui donne la version
//         getline(monFlux, line);

//         //On lit la ligne contenant la width et height du .PGM et on les places dans les variables
//         getline(monFlux,line);
//         char *pch;
//         char str[line.length()+1];
//         strcpy(str, line.c_str());
//         pch=strtok(str," ");

//         while (pch != NULL){
//             if(iterator==0){
//                 *width=atoi(pch);
//                 iterator++;
//             }else{
//                 *height=atoi(pch);
//             }
//             pch = strtok (NULL, " ");
//         }
        
//         //On recupere le niveau de gris
//         getline(monFlux,line);
//         *grayLvl=atoi(line.c_str());

//         //On recupere tout le tableau de la map
//         int *map=(int*) malloc(sizeof(int)*(*height * *width));
//         iterator=0;
//         for(int i=0; i<*height; i++){
            
//             getline(monFlux,line);
//             char strcmp[line.length()+1];
//             strcpy(strcmp, line.c_str());
//             pch=strtok(strcmp," ");

//             while(pch != NULL){
//                 map[iterator]=atoi(pch);
//                 pch = strtok (NULL, " ");
//                 iterator++;
//             }
//         }
//         return map;
//     }else{
//         cout << "ERREUR: Impossible d'ouvrir le fichier PGM en lecture." << endl;
//         return NULL;
//     }
// }

//Lecture d'un pgm genere par gimp

int* lecturePGM(string nameHeightMap, int *width, int *height, int *grayLvl){
    string line;
    int iterator=0;
    ifstream monFlux("./assets/"+nameHeightMap);
    if(monFlux){
        //On skip la premiere ligne qui donne la version
        getline(monFlux, line);
        getline(monFlux, line);

        //On lit la ligne contenant la width et height du .PGM et on les places dans les variables
        getline(monFlux,line);
        char *pch;
        char str[line.length()+1];
        strcpy(str, line.c_str());
        pch=strtok(str," ");

        while (pch != NULL){
            if(iterator==0){
                *width=atoi(pch);
                iterator++;
            }else{
                *height=atoi(pch);
            }
            pch = strtok (NULL, " ");
        }
        
        //On recupere le niveau de gris
        getline(monFlux,line);
        *grayLvl=atoi(line.c_str());

        //On recupere tout le tableau de la map
        string ligne[*height * *width];
        int *map=(int*) malloc(sizeof(int)*(*height * *width));
        for(int i=0; i<*height * *width; i++){
            getline(monFlux,ligne[i]);
            map[i]=atoi(ligne[i].c_str());
        }
        return map;
    }else{
        cout << "ERREUR: Impossible d'ouvrir le fichier PGM en lecture." << endl;
        return NULL;
    }
}


