#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/draw.h"
#include "../include/calcul.h"

// prend les coordonne du quadtree et appelle les fonction de dessin en appliquant le LOD
void drawQuadTreeLOD(Node* quadtree, Light soleil, GLuint texture[], Camera camera, float* map, int mapWidth, int mapHeight,float grayLvl, float zFar, float angleView, float grayLvlRatio, Timac *timac) {
    if (FrustumCulling(camera, zFar, angleView, *quadtree) == 1) {
        if (quadtree->isLeaf()) {
            updateZ(quadtree, map, mapWidth, mapHeight, grayLvl);
            textureToDrawTriangles(*quadtree, soleil, texture, grayLvlRatio);
        } else if (distanceFromQuad(*quadtree, camera)>(2*timac->Xsize/(quadtree->depth+1))) {
            updateZ(quadtree, map, mapWidth,mapHeight,  grayLvl);
            textureToDrawTriangles(*quadtree, soleil, texture, grayLvlRatio);

            //Modification de la map pour éviter les cracks
            //iA, iB, iC, iD : indice des points dans la map
            Point3D A = quadtree->getPointAEnPixels();
            int iA = (A.x)*mapWidth+(A.y);
            Point3D B = quadtree->getPointBEnPixels();
            int iB = (B.x)*mapWidth+(B.y);
            Point3D C = quadtree->getPointCEnPixels();
            int iC = (C.x)*mapWidth+(C.y);
            Point3D D = quadtree->getPointDEnPixels();
            int iD = (D.x)*mapWidth+(D.y);

            float deltaAB = (float) (iB - iA);
            float zAB = B.z - A.z;
            for (int i = iA; i<iB; i++) {
                map[i] = (A.z + (i-iA)*(zAB/deltaAB))*grayLvl;
            }
            float deltaDC = (float) (iC - iD);
            float zDC = C.z - D.z;
            for (int i = iD; i<iC; i++) {
                map[i] = (D.z + (i-iD)*(zDC/deltaDC))*grayLvl;
            }
            float deltaAD = (float) ((iD-iA)/mapWidth);
            float zAD = D.z - A.z;
            for (int i = iA; i<iD; i+= mapWidth) {
                map[i] = (A.z + ((i-iA)/((float)mapWidth)*(zAD/deltaAD)))*grayLvl;
            }
            float deltaBC = (float) ((iC-iB)/mapWidth);
            float zBC = C.z - B.z;
            for (int i = iB; i<iC; i+= mapWidth) {
                map[i] = (B.z + ((i-iB)/((float)mapWidth)*(zBC/deltaBC)))*grayLvl;
            }
        } else {
            Node** tabEnfantOrdonne = (Node**) malloc(sizeof(Node*)*4);
            orderChildren(*quadtree, tabEnfantOrdonne, camera);
            drawQuadTreeLOD(tabEnfantOrdonne[3], soleil, texture, camera, map, mapWidth, mapHeight, grayLvl, zFar, angleView, grayLvlRatio, timac);
            drawQuadTreeLOD(tabEnfantOrdonne[2], soleil, texture, camera, map, mapWidth, mapHeight, grayLvl, zFar, angleView, grayLvlRatio, timac);
            drawQuadTreeLOD(tabEnfantOrdonne[1], soleil, texture, camera, map, mapWidth, mapHeight, grayLvl, zFar, angleView, grayLvlRatio, timac);
            drawQuadTreeLOD(tabEnfantOrdonne[0], soleil, texture, camera, map, mapWidth, mapHeight, grayLvl, zFar, angleView, grayLvlRatio, timac);
        }
    }
}

// Affichage triangles texturés
void drawTriangle(Point3D s1, Point3D s2, Point3D s3, Light Soleil, GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_TRIANGLES);
        ColorRGB couleurS1 = illuminationLambert(s1, s2, s3, Soleil);
        glColor3f(couleurS1.r, couleurS1.g, couleurS1.b);
        glTexCoord2f(0.,0.);
        glVertex3f(s1.x, s1.y, s1.z); 

        ColorRGB couleurS2 = illuminationLambert(s2, s3, s1, Soleil);
        glColor3f(couleurS2.r, couleurS2.g, couleurS2.b);
        glTexCoord2f(1.,0.);
        glVertex3f(s2.x, s2.y, s2.z);

        ColorRGB couleurS3 = illuminationLambert(s3, s1, s2, Soleil);
        glColor3f(couleurS3.r, couleurS3.g, couleurS3.b);
        glTexCoord2f(1.,1.);
        glVertex3f(s3.x, s3.y, s3.z);
    glEnd();   

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

//Choix des texture des triangles selon la hauteur
void textureToDrawTriangles(Node noeud, Light Soleil, GLuint texture[], float grayLvlRatio){
    Point3D centre=topOfTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC());
    if(centre.z>=0 && centre.z<0.2*grayLvlRatio){
        drawTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), Soleil, texture[0]);
    }else if(centre.z>=0.2*grayLvlRatio && centre.z<0.4*grayLvlRatio){
        drawTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), Soleil, texture[1]);
    }else if(centre.z>=0.4*grayLvlRatio && centre.z<0.6*grayLvlRatio){
        drawTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), Soleil, texture[2]);
    }else if(centre.z>=0.6*grayLvlRatio && centre.z<0.8*grayLvlRatio){
        drawTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), Soleil, texture[3]);
    }else if(centre.z>=0.8*grayLvlRatio){
        drawTriangle(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), Soleil, texture[4]);
    }

    centre=topOfTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA());
    if(centre.z>=0 && centre.z<0.2*grayLvlRatio){
        drawTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), Soleil, texture[0]);
    }else if(centre.z>=0.2*grayLvlRatio && centre.z<0.4*grayLvlRatio){
        drawTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), Soleil, texture[1]);
    }else if(centre.z>=0.4*grayLvlRatio && centre.z<0.6*grayLvlRatio){
        drawTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), Soleil, texture[2]);
    }else if(centre.z>=0.6*grayLvlRatio && centre.z<0.8*grayLvlRatio){
        drawTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), Soleil, texture[3]);
    }else if(centre.z>=0.8*grayLvlRatio){
        drawTriangle(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), Soleil, texture[4]);
    }
    
}

//Prend les coordonne du quadtree et appelle les fonction de dessin filaire en appliquant le LOD
void drawQuadTreeLinesLOD(Node* quadtree, Camera camera, float* map, int mapWidth, int mapHeight, float zFar, float angleView, float grayLvl, Timac *timac) {
    float thickness = norm(createVectorFromPoints(quadtree->getPointA(), quadtree->getPointB()));
    if (FrustumCulling(camera, zFar, angleView, *quadtree) == 1) {
        if (quadtree->isLeaf()) {
            updateZ(quadtree, map, mapWidth, mapHeight, grayLvl);
            drawTrianglesLines(*quadtree, thickness);
        } else if (distanceFromQuad(*quadtree, camera)>(1.3*(timac->Xsize)/(quadtree->depth+1))) {
            updateZ(quadtree, map, mapWidth, mapHeight, grayLvl);
            drawTrianglesLines(*quadtree, thickness);

            //Modification de la map pour éviter les cracks
            //iA, iB, iC, iD : indice des points dans la map
            Point3D A = quadtree->getPointAEnPixels();
            int iA = (A.x)*mapWidth+(A.y);
            Point3D B = quadtree->getPointBEnPixels();
            int iB = (B.x)*mapWidth+(B.y);
            Point3D C = quadtree->getPointCEnPixels();
            int iC = (C.x)*mapWidth+(C.y);
            Point3D D = quadtree->getPointDEnPixels();
            int iD = (D.x)*mapWidth+(D.y);

            float deltaAB = (float) (iB - iA);
            float zAB = B.z - A.z;
            for (int i = iA; i<iB; i++) {
                map[i] = (A.z + (i-iA)*(zAB/deltaAB))*grayLvl;
            }
            float deltaDC = (float) (iC - iD);
            float zDC = C.z - D.z;
            for (int i = iD; i<iC; i++) {
                map[i] = (D.z + (i-iD)*(zDC/deltaDC))*grayLvl;
            }
            float deltaAD = (float) ((iD-iA)/mapWidth);
            float zAD = D.z - A.z;
            for (int i = iA; i<iD; i+= mapWidth) {
                map[i] = (A.z + ((i-iA)/((float)mapWidth)*(zAD/deltaAD)))*grayLvl;
            }
            float deltaBC = (float) ((iC-iB)/mapWidth);
            float zBC = C.z - B.z;
            for (int i = iB; i<iC; i+= mapWidth) {
                map[i] = (B.z + ((i-iB)/((float)mapWidth)*(zBC/deltaBC)))*grayLvl;
            }
        } else {
            Node** tabEnfantOrdonne = (Node**) malloc(sizeof(Node*)*4);
            orderChildren(*quadtree, tabEnfantOrdonne, camera);
            drawQuadTreeLinesLOD(tabEnfantOrdonne[3], camera, map, mapWidth, mapHeight, zFar, angleView, grayLvl, timac);
            drawQuadTreeLinesLOD(tabEnfantOrdonne[2], camera, map, mapWidth, mapHeight, zFar, angleView, grayLvl, timac);
            drawQuadTreeLinesLOD(tabEnfantOrdonne[1], camera, map, mapWidth, mapHeight, zFar, angleView, grayLvl, timac);
            drawQuadTreeLinesLOD(tabEnfantOrdonne[0], camera, map, mapWidth, mapHeight, zFar, angleView, grayLvl, timac);
        }
    }
}

//Dessine le noeud en affichage filaire
void drawTriangleLines(Point3D s1, Point3D s2, Point3D s3, float thickness) {
    glLineWidth(thickness);
    glBegin(GL_LINES);
        
        glColor4f(1.-1./thickness, 1., 1./thickness, 1./thickness);
        glVertex3f(s1.x, s1.y, s1.z); 
        glVertex3f(s2.x, s2.y, s2.z);

        glVertex3f(s2.x, s2.y, s2.z);
        glVertex3f(s3.x, s3.y, s3.z);

        glVertex3f(s3.x, s3.y, s3.z);
        glVertex3f(s1.x, s1.y, s1.z); 
    glEnd();   
}

//Appel drawTriangleLine sur les points du noeud
void drawTrianglesLines(Node noeud, float thickness){
    drawTriangleLines(noeud.getPointA(), noeud.getPointB(), noeud.getPointC(), thickness);
    drawTriangleLines(noeud.getPointC(), noeud.getPointD(), noeud.getPointA(), thickness);
}

//Dessine un Billboard 
void drawBillboard(float phi, GLuint texture, Point3D scale, Light Soleil) {
            glRotatef(phi*(360/6.18),0.,0.,1.);
            glScalef(scale.x,scale.y,scale.z);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBegin(GL_QUADS);

                //ColorRGB eclairage = illuminationLambert(createPoint(0.,-0.5,1.), createPoint(0.,0.5,1.), createPoint(0.,0.5,0.), Soleil);
                ColorRGB eclairage = illuminationLambert(createPoint(0.5,0.,0.), createPoint(0.,0.,0.), createPoint(0.,0.5,0.), Soleil);
                glColor3f(eclairage.r,eclairage.g,eclairage.b);

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
}

//DRAW_MENU
void drawMenu(GLuint texture, Camera camera, float phi, float teta){
    
    glPushMatrix();
        glTranslatef(camera.posCam.x, camera.posCam.y, camera.posCam.z);
        Vector3D dirRegard = createVectorFromPoints(camera.posCam, camera.viseCam);
        dirRegard = normalize(dirRegard);
        dirRegard = multVector(dirRegard, 0.9);
        glTranslatef(dirRegard.x, dirRegard.y, dirRegard.z);
        glRotatef(phi*(360/6.18),0.,0.,1.);
        glRotatef(-teta*(360/6.18), 0., 1., 0.);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);     

        glTexCoord2f(0.,0.);
        glVertex3f(0.,1.,0.6);        

        glTexCoord2f(1.,0.);
        glVertex3f(0.,-1.,0.6);
        
        glTexCoord2f(1.,1.);
        glVertex3f(0.,-1.,-0.6);

        glTexCoord2f(0.,1.);
        glVertex3f(0.,1.,-0.6);
        
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

//Dessine l'origine
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

//Dessine la Skybox
void drawCenteredBox(float length, ColorRGB couleurCiel, GLuint* textureSky) {
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);
    glEnable(GL_TEXTURE_2D);
    
    float l = length/2;
    //glColor3f(1., 1., 1.);
    glColor3f(couleurCiel.r, couleurCiel.g, couleurCiel.b);
    glBindTexture(GL_TEXTURE_2D, textureSky[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.,1.);
    glVertex3f(-l , l, -l);
    glTexCoord2f(0.,0.);
    glVertex3f(-l , l, l);
    glTexCoord2f(1.,0.);
    glVertex3f(-l , -l, l);
    glTexCoord2f(1.,1.);
    glVertex3f(-l , -l, -l);

    glTexCoord2f(1.,1.);
    glVertex3f(-l , l, -l);
    glTexCoord2f(1.,0.);
    glVertex3f(-l , l, l);
    glTexCoord2f(0.,0.);
    glVertex3f(l , l, l);
    glTexCoord2f(0.,1.);
    glVertex3f(l , l, -l);

    glTexCoord2f(1.,1.);
    glVertex3f(l , -l, -l);
    glTexCoord2f(1.,0.);
    glVertex3f(l , -l, l);
    glTexCoord2f(0.,0.);
    glVertex3f(l , l, l);
    glTexCoord2f(0.,1.);
    glVertex3f(l , l, -l);

    glTexCoord2f(1.,1.);
    glVertex3f(l , -l, -l);
    glTexCoord2f(1.,0.);
    glVertex3f(l , -l, l);
    glTexCoord2f(0.,0.);
    glVertex3f(-l , -l, l);
    glTexCoord2f(0.,1.);
    glVertex3f(-l , -l, -l);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureSky[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.,0.);
    glVertex3f(-l , -l, -l);
    glTexCoord2f(1.,0.);
    glVertex3f(-l , l, -l);
    glTexCoord2f(1.,1.);
    glVertex3f(l , l, -l);
    glTexCoord2f(0.,1.);
    glVertex3f(l , -l, -l);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureSky[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.,0.);
    glVertex3f(-l , -l, l);
    glTexCoord2f(1.,0.);
    glVertex3f(-l , l, l);
    glTexCoord2f(1.,1.);
    glVertex3f(l , l, l);
    glTexCoord2f(0.,1.);
    glVertex3f(l , -l, l);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);   

    glEnd();

    glColor3fv(currentColor);
}

//Prend un nom de texture en parametre et retourne la texture associee
GLuint generateTexture(char* name){
    GLuint textureId;
    
    SDL_Surface* image = IMG_Load(name);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

