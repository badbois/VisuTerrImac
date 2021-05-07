#include "read.h"
using namespace std;


int main(int argc, char const *argv[]){
    string nameHeightMap;
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
            cout << map[i]<< endl;
        }
       
    }
    return EXIT_SUCCESS;
}
