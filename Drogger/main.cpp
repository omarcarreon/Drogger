//
//  main.cpp
//  Drogger
//
//  Created by Omar Carreon and Ricardo Perez
//  Copyright © 2016 Omar Carreon and Ricardo Perez. All rights reserved.
//


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <math.h>
#include <string>
#include <iostream>
using namespace std;
#include <stdlib.h>
#include "ImageLoader.hpp"
#include "glm.h"

#include "SOIL.h"


#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"


//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gameFail = NULL;
Mix_Chunk *levelcomplete = NULL;
Mix_Chunk *gamewin = NULL;
Mix_Chunk *pierdevida = NULL;
bool success = true;

//Cantidad de modelos y sus ids
#define MODEL_COUNT 30
#define DROGGER_MOD 0
#define PLAYER_MOD 1
#define BUSSTOP_MOD 2
#define BEER_MOD 3
#define DRUG_MOD 4
#define GUN_MOD 5
#define CIGAR_MOD 6
#define SEMAFORO_MOD 7
#define GUY_MOD 8
#define PLAYSET_MOD 9
#define BENCH_MOD 10
#define SCHOOL_MOD 11
#define PHONE_MOD 12
#define FB_MOD 13
#define WHATSAPP_MOD 14

// Cantidad de texturas y sus ids
const int TEXTURE_COUNT=36;
static GLuint texName[TEXTURE_COUNT];
#define GAMEOVER_TEXTURE 10
#define WINLEVEL1_TEXTURE 11
#define WINLEVEL2_TEXTURE 12
#define COMPLETEGAME_TEXTURE 13
#define CREDITS_TEXTURE 15
#define MAINMENU_TEXTURE 4
#define INSTRUCTIONS_TEXTURE 5
#define LEVEL1 7
#define LEVEL2 8
#define LEVEL3 9

int actualTexture = MAINMENU_TEXTURE; // Inicia juego con menu principal

/////////////
string fullPath = __FILE__;
using namespace std;

GLMmodel models[MODEL_COUNT];

int screenWidth = 1000, screenHeight = 700;
double tableroWidth=100.0, tableroHeigth=100.0;
int angulo = 0;
int enteroGlobal = 0;

int vidastotal = 3; // Inicializa numero de vidas

// Posiciones de jugador y metas del nivel
double posXPersonaje = 0.0, posYPersonaje = -85.0;
double posXBusStop = 0.0, posYBusStop = 120.0;
double posXSemaforo = 0.0, posYSemaforo = 120.0;
double posXSchool = 0.0, posYSchool = 130.0;


// Obstaculos Nivel 1
// Determinan la direccion del movimiento
bool beerAscending = true;
bool drugAscending = true;
bool cigarAscending = true;

// Posiciones obstaculos nivel 1
double posXCigarUp = 0, posYCigarUp = -55.0;
double posXCigarDown = 0, posYCigarDown = -75.0;
double posXBeerUp = -140.0, posYBeerUp = 0.0;
double posXBeerDown = 140.0, posYBeerDown = -20.0;
double posXDrugUp = 130, posYDrugUp = 60;
double posXDrugDown = -130, posYDrugDown = 40;

// Obstaculos Nivel 2
// Determinan la direccion del movimiento
bool gunAscending = true;
bool manAscending = true;
// Posiciones obstaculos nivel 1
double posXGunUp = -130.0, posYGunUp = -15.0;
double posXGunDown = 130.0, posYGunDown = -35.0;
double posXMan = 0.0, posYMan = -60.0;

// Obstaculos Nivel 3
// Determinan la direccion del movimiento
bool fbAscending = true;
bool phoneAscending = true;
bool droggerAscending = true;
// Posiciones obstaculos nivel 1
double posXPhoneUp = -130.0, posYPhoneUp = -45.0;
double posXPhoneDown = 130.0, posYPhoneDown = -70.0;
double posXDrogger = 0.0, posYDrogger = -15.0;
double posXWhatsapp = 130.0, posYWhatsapp = 20.0;
double posXFbDown = -130.0, posYFbDown = 0.0;

bool finished = false; // Indica si el juego termino
bool start = false; // Indica si esta corriendo el juego
bool play = false; // Indica si inicia o encuentra en un nivel del juego
bool running = false; // Indica si timer esta corriendo

GLfloat variableLightPosition = 0.0f;

//le borramos el exceso para solo obtener el Path padre
void getParentPath()
{
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i,1);
    }
}

float ambiente[][4]={
    {0.0215, 0.1745, 0.0215, 1}, //esmeralda
    {0.1745, 0.01175, 0.01175,1}, //ruby
    {0.135, 0.2225, 0.1575,1},
    {0.05375, 0.05, 0.06625,1},
    {0.24725, 0.1995, 0.0745,1}
};

float difuso[][4]={
    {0.07568, 0.61424, 0.07568,1},
    {0.61424, 0.04136, 0.04136,1},
    {0.54, 0.89, 0.63,1},
    {0.18275, 0.17, 0.22525,1},
    {0.75164, 0.60648, 0.22648}
};

float especular[][4]={
    {0.633, 0.727811, 0.633,1},
    {0.727811, 0.626959, 0.626959,1},
    {0.316228, 0.316228, 0.316228,1},
    {0.332741, 0.328634, 0.346435,1},
    {0.628281, 0.555802, 0.366065}
};

float shine[]={1.0,1.0,0.1,0.3, 0.4};


void lucesMaterial(int i)
{
    //Asigna los apropiados materiales a las superficies
    glMaterialfv(GL_FRONT,GL_AMBIENT,ambiente[i]);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,difuso[i]);
    glMaterialfv(GL_FRONT,GL_SPECULAR,especular[i]);
    glMaterialf(GL_FRONT,GL_SHININESS,shine[i]*128.0);
    // asigna la apropiada fuente de luz
    GLfloat lightIntensity[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_position[] = {2.0f, variableLightPosition, 3.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION,light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,lightIntensity);
    //asigna la c√°mara
    //comienza el dibujo
    
}

void draw3dString (void *font, char *s, float x, float y, float z)
{
    unsigned int i;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glScaled(1.0, 1.0, 1.0);
    glLineWidth(1);
    glColor3f(0.0, 0.0, 0.0);
    for (i = 0; i < s[i] != '\0'; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, s[i]);
                 
    }
    glPopMatrix();
}

void movimientoMan(){
    if (posXMan <= 140 && posXMan >= -140 && manAscending) {
        posXMan += 10;
    }
    else if (posXMan >= 140) {
        posXMan -= 10;
        manAscending = false;
    }
    else if (posXMan <= 140 && posXMan >= -140 && !manAscending) {
        posXMan -= 10;
    }
    else if (posXMan <= -140) {
        posXMan += 10;
        manAscending = true;
    }
}

/********************************
* Movimiento del modelo Beer
********************************/
void movimientoBeer() {
    if (posXBeerUp <= 140 && posXBeerUp >= -140 && beerAscending) {
        posXBeerUp += 7;
    }
    else if (posXBeerUp >= 140) {
        posXBeerUp -= 7;
        beerAscending = false;
    }
    else if (posXBeerUp <= 140 && posXBeerUp >= -140 && !beerAscending) {
        posXBeerUp -= 7;
    }
    else if (posXBeerUp <= -140) {
        posXBeerUp += 7;
        beerAscending = true;
    }
}

/********************************
 * Movimiento del modelo Drug
 ********************************/
void movimientoDrug() {
    if (posXDrugDown <= 140 && posXDrugDown >= -140 && drugAscending) {
        posXDrugDown += 7;
    }
    else if (posXDrugDown >= 140) {
        posXDrugDown -= 7;
        drugAscending = false;
    }
    else if (posXDrugDown <= 140 && posXDrugDown >= -140 && !drugAscending) {
        posXDrugDown -= 7;
    }
    else if (posXDrugDown <= -140) {
        posXDrugDown += 7;
        drugAscending = true;
    }
}

/********************************
 * Movimiento del modelo Cigar
 ********************************/
void movimientoCigar() {
    if (posXCigarDown <= 140 && posXCigarDown >= -140 && cigarAscending) {
        posXCigarDown += 7;
    }
    else if (posXCigarDown >= 140) {
        posXCigarDown -= 7;
        cigarAscending = false;
    }
    else if (posXCigarDown <= 140 && posXCigarDown >= -140 && !cigarAscending) {
        posXCigarDown -= 7;
    }
    else if (posXCigarDown <= -140) {
        posXCigarDown += 7;
        cigarAscending = true;
    }
}

/********************************
 * Movimiento del modelo Gun
 ********************************/
void movimientoGun() {
    if (posXGunUp <= 140 && posXGunUp >= -140 && gunAscending) {
        posXGunUp += 10;
    }
    else if (posXGunUp >= 140) {
        posXGunUp -= 10;
        gunAscending = false;
    }
    else if (posXGunUp <= 140 && posXGunUp >= -140 && !gunAscending) {
        posXGunUp -= 10;
    }
    else if (posXGunUp <= -140) {
        posXGunUp += 10;
        gunAscending = true;
    }
}

/********************************
 * Movimiento del modelo Phone
 ********************************/
void movimientoPhone(){
    if (posXPhoneUp <= 140 && posXPhoneUp >= -140 && phoneAscending) {
        posXPhoneUp += 13;
    }
    else if (posXPhoneUp >= 140) {
        posXPhoneUp -= 13;
        phoneAscending = false;
    }
    else if (posXPhoneUp <= 140 && posXPhoneUp >= -140 && !phoneAscending) {
        posXPhoneUp -= 13;
    }
    else if (posXPhoneUp <= -140) {
        posXPhoneUp += 13;
        phoneAscending = true;
    }
}

/********************************
 * Movimiento del modelo Drogger
 ********************************/
void movimientoDrogger(){
    if (posXDrogger <= 140 && posXDrogger >= -140 && droggerAscending) {
        posXDrogger += 13;
    }
    else if (posXDrogger >= 140) {
        posXDrogger -= 13;
        droggerAscending = false;
    }
    else if (posXDrogger <= 140 && posXDrogger >= -140 && !droggerAscending) {
        posXDrogger -= 13;
    }
    else if (posXDrogger <= -140) {
        posXDrogger += 13;
        droggerAscending = true;
    }
}

/**************************************
 * Movimiento del modelo FB (Facebook)
 **************************************/
void movimientoFb(){
    if (posXFbDown <= 140 && posXFbDown >= -140 && fbAscending) {
        posXFbDown += 13;
    }
    else if (posXFbDown >= 140) {
        posXFbDown -= 13;
        fbAscending = false;
    }
    else if (posXFbDown <= 140 && posXFbDown >= -140 && !fbAscending) {
        posXFbDown -= 13;
    }
    else if (posXFbDown <= -140) {
        posXFbDown += 13;
        fbAscending = true;
    }
}


/****************************************************************************
 * Makes the image into a texture, and returns the id of the texture
 ****************************************************************************/
void loadTexture(Image* image,int k)
{
    
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
}

/****************************************************************************
 * Carga imagenes/texturas de otros tipos utilizando la libreria SOIL
 ****************************************************************************/
int LoadGLTextures(const char* image, int i)
{
    texName[i] = SOIL_load_OGL_texture(image, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO
                                       | SOIL_FLAG_MIPMAPS
                                       | SOIL_FLAG_MULTIPLY_ALPHA
                                       | SOIL_FLAG_COMPRESS_TO_DXT
                                       | SOIL_FLAG_DDS_LOAD_DIRECT
                                       | SOIL_FLAG_INVERT_Y);
    
    if(texName[i] == 0){
        cout <<"Not found" << endl;
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, texName[i]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    return true;
}

/****************************************************************************
 * Renderea todas las texturas
 ****************************************************************************/
void initRendering()
{
    //Declaración del objeto Image
    Image* image;
    GLuint i=0;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(TEXTURE_COUNT, texName); //Make room for our texture
    
    
    char  ruta[200];
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/grass.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/road.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/sand.bmp");
    image = loadBMP(ruta);loadTexture(image,i++);
    
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/heartfull.bmp");
    LoadGLTextures(ruta,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/menu1.bmp");
    LoadGLTextures(ruta,MAINMENU_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/instrucciones1.bmp");
    LoadGLTextures(ruta,INSTRUCTIONS_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/ganarNivel1.bmp");
    LoadGLTextures(ruta,WINLEVEL1_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/ganarNivel2.bmp");
    LoadGLTextures(ruta,WINLEVEL2_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/perdernivel1.bmp");
    LoadGLTextures(ruta,GAMEOVER_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/terminarJuego1.bmp");
    LoadGLTextures(ruta,COMPLETEGAME_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/creditos1.bmp");
    LoadGLTextures(ruta,CREDITS_TEXTURE);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/background.jpg");
    LoadGLTextures(ruta,14);
    
    delete image;
}

/****************************************************************************
 * Funcion Reshape
 ****************************************************************************/
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-tableroWidth, tableroWidth, -tableroHeigth, tableroHeigth, 1.0, 10.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 2.8, 0, 0, 0, 0, 5, 10);
    
}
// ****Genera el formato del timer ****//
// ************************************//
string formato(int i){
    int seconds, minutes ,mili;
    string cadena;
    
    mili = i%10;
    minutes = floor((i/10)/60);
    seconds = (i/10) - minutes * 60;
    if (seconds <10){
        cadena =to_string(minutes) + ":0" + to_string(seconds) + ":" + to_string(mili);
    } else {
        cadena = to_string(minutes) + ":" + to_string(seconds) + ":" + to_string(mili);
    }
    return cadena;
}

// ****** Dibuja timer ********//
// ****************************//
void displayTimer(){
    
    GLint x;
    glPushMatrix();
    char displaycadena[200]="";
    string cadena = "";
    cadena = formato(enteroGlobal);
    
    sprintf(displaycadena, "%s",cadena.c_str());
    glColor3f(0.0, 0.0, 0.0);
    draw3dString(GLUT_STROKE_MONO_ROMAN,displaycadena,7.0, 5.0, 0.1);
    glPopMatrix();
}




/****************************************************************************
 * Dibuja textura del Cesped
 ****************************************************************************/
void dibujaCesped(double y,double height, double z, double width){
    glPushMatrix();
    glColor3ub(0,255, 0);
    glTranslated(0, y, z);
    glScalef(width, height, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del cesped
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -1.0f, 0);
    
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -1.0f, 0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.3f, 0);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.3f, 0);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutPostRedisplay();
}

/****************************************************************************
 * Dibuja textura de Area Segura (Arena)
 ****************************************************************************/
void dibujaAreaSegura(double y, double z, double height, double width) {
    glPushMatrix();
    glColor3ub(255,255,224);
    glTranslated(0, y, z);
    glScalef(width, height, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    //Elegir la textura del area segura - arena
    glBindTexture(GL_TEXTURE_2D, texName[2]);
    glBegin(GL_QUADS);
    
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -1.0f, 0);
    
    glTexCoord2f(3.0f, 0.0f);
    glVertex3f(0.5f, -1.0f, 0);
    
    glTexCoord2f(3.0f, 1.0f);
    glVertex3f(0.5f, 0.3f, 0);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.3f, 0);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
}

/****************************************************************************
 * Dibuja textura de la Calle
 ****************************************************************************/
void dibujaCalle(double y, double rayasY, double z, double width, double height) {
    glPushMatrix();
    glColor3ub(128,128, 128);
    glTranslated(0, y, z);
    glScalef(width, height, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura de la calle
    glBindTexture(GL_TEXTURE_2D, texName[1]);
    
    glBegin(GL_QUADS);
    float roadw = -0.5f;
    for (int i=0;i<4; i++) {
        
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(roadw, -0.62f, 0);
        
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(roadw+0.25, -0.62f, 0);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(roadw+0.25, 0.3f, 0);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(roadw, 0.3f, 0);
        roadw += 0.25;
    }
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutPostRedisplay();
}

/****************************************************************************
 * Dibuja modelo de Gregorio - Personaje Principal
 ****************************************************************************/
void dibujaPersonaje(double posX, double posY) {
    posXPersonaje = posX;
    posYPersonaje = posY;
    glClear(GL_DEPTH_BUFFER_BIT); // Se usa para limpiar buffer depth (que los modelos se pongan enfrente de todo)
    
    glPushMatrix();
    
    
    glTranslated(posXPersonaje, posYPersonaje, 0);
    glRotatef(0.8, 1, 0, 0);
    glRotatef(180, 0,1, 0);
    glScaled(20, 20, 0.1);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR);
    
    
    glPopMatrix();

}

/****************************************************************************
 * Colision del personaje con la pistola
 ****************************************************************************/
void colisionGun(){
    if (posYGunUp == posYPersonaje ){
        if (fabs(posXGunUp-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXGunUp == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
    
    if (posYGunDown == posYPersonaje){
        if (fabs(posXGunDown-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXGunDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con el hombre del Nivel 2
 ****************************************************************************/
void colisionMan(){
    int offset = 5;
    if (posYMan == posYPersonaje + offset){
        if (fabs(posXMan-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        } else if (posXMan == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con la botella de cerveza
 ****************************************************************************/
void colisionBeer() {
    int offset = 5;
    if (posYBeerUp == posYPersonaje + offset){
        if (fabs(posXBeerUp-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXBeerUp == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
    
    if (posYBeerDown == posYPersonaje + offset){
        if (fabs(posXBeerDown-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXBeerDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con la droga
 ****************************************************************************/
void colisionDrug() {
    int offset = 15;
    if (posYDrugUp == posYPersonaje - offset){
        if (fabs(posXDrugUp-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXDrugUp == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
    
    if ((posYDrugDown == posYPersonaje - offset) || (posYPersonaje -posYDrugDown == 5)){
        if (fabs(posXDrugDown-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXDrugDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con el cigarro
 ****************************************************************************/
void colisionCigar() {
    int offset = 10;
    
    if (posYCigarUp == posYPersonaje - offset){
        if (fabs(posXCigarUp-posXPersonaje) <= 6) {
            vidastotal--;
            dibujaPersonaje(0, -85);
            Mix_PlayChannel( 0, pierdevida, 0 );
            
        } else if (posXCigarUp == 0 && posXPersonaje == 0) {
            vidastotal--;
            dibujaPersonaje(0, -85);
            Mix_PlayChannel( 0, pierdevida, 0 );
        }
    }
    
    if (posYCigarDown == posYPersonaje - offset){
        if (fabs(posXCigarDown-posXPersonaje) <= 6) {
            vidastotal--;
            dibujaPersonaje(0, -85);
            Mix_PlayChannel( 0, pierdevida, 0 );
            
        } else if (posXCigarDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            dibujaPersonaje(0, -85);
            Mix_PlayChannel( 0, pierdevida, 0 );
        }
    }
}

/****************************************************************************
 * Colision del personaje con la parada de autobus (Meta del nivel 1)
 ****************************************************************************/
void colisionBusStop() {
    if (posYPersonaje >= 95){
        if (posXPersonaje >= -50 && posXPersonaje <= 40) {
            play = false;
            Mix_HaltMusic();
            Mix_PlayChannel( 0, levelcomplete, 0 );
            actualTexture = WINLEVEL1_TEXTURE;
            variableLightPosition = 0.0f;
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con el semaforo (Meta del nivel 2)
 ****************************************************************************/
void colisionSemaforo() {
    if (posYPersonaje == 45){
        if (posXPersonaje == 50) {
            play = false;
            Mix_HaltMusic();
            Mix_PlayChannel( 0, levelcomplete, 0 );
            actualTexture = WINLEVEL2_TEXTURE;
            variableLightPosition = 0.0f;
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con la escuela (Meta del nivel 3)
 ****************************************************************************/
void colisionSchool() {
    if (posYPersonaje >= 75){
        if (posXPersonaje >= -110 && posXPersonaje <= 110) {
            Mix_HaltMusic();
            actualTexture = COMPLETEGAME_TEXTURE;
            Mix_PlayChannel(0, gamewin, 0);
            play = false;
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con el celular
 ****************************************************************************/
void colisionPhone() {
    int offset = 5;
    if (posYPhoneUp == posYPersonaje || posYPhoneUp == posYPersonaje - 10){
        if (fabs(posXPhoneUp-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
            
        } else if (posXPhoneUp == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
    if (posYPhoneDown == posYPersonaje - offset){
        if (fabs(posXPhoneDown-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXPhoneDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con Drogger (personaje del nivel 3)
 ****************************************************************************/
void colisionDrogger() {
    if (posYDrogger == posYPersonaje + 10){
        if (fabs(posXDrogger-posXPersonaje) <= 13) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXDrogger == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
        
    }
}

/****************************************************************************
 * Colision del personaje con el logo de facebook
 ****************************************************************************/
void colisionFb() {
    int offset = 5;
    if (posYFbDown == posYPersonaje - offset){
        if (fabs(posXFbDown-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXFbDown == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Colision del personaje con el logo de whatsapp
 ****************************************************************************/
void colisionWhatsapp() {
    int offset1 = 5, offset2 = 15;
    if (posYWhatsapp == posYPersonaje - offset1 || posYWhatsapp == posYPersonaje - offset2){
        if (fabs(posXWhatsapp-posXPersonaje) <= 6) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
            
        } else if (posXWhatsapp == 0 && posXPersonaje == 0) {
            vidastotal--;
            Mix_PlayChannel( 0, pierdevida, 0 );
            dibujaPersonaje(0, -85);
        }
    }
}

/****************************************************************************
 * Dibuja modelo de Parada de autobus (camion)
 ****************************************************************************/
void dibujaBusStop() {
    glClear(GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslated(posXBusStop,posYBusStop, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(60,60, 0);
    glRotatef(-90, 0, 1, 0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    lucesMaterial(0);
    glmDraw(&models[BUSSTOP_MOD], GLM_COLOR | GL_TEXTURE | GLM_SMOOTH);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de Semaforo
 ****************************************************************************/
void dibujaSemaforo() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    lucesMaterial(1);
    glPushMatrix();

    glTranslated(posXSemaforo,posYSemaforo, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(70,120, 1);
    
    glmDraw(&models[SEMAFORO_MOD], GLM_COLOR | GL_TEXTURE | GLM_SMOOTH);
    
    glPopMatrix();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

/****************************************************************************
 * Dibuja modelo de juegos del parque
 ****************************************************************************/
void dibujaPlayset() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(-100,20, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(30,40, 0.1);
    glRotatef(-58, 0, 1, 0);
    
    glmDraw(&models[PLAYSET_MOD], GLM_COLOR | GLM_SMOOTH);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de banca del parque
 ****************************************************************************/
void dibujaBench() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(100,15, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(30,40, 0.1);
    glRotatef(-60, 0, 1, 0);
    glRotatef(-7, 0, 0, 1);
    glmDraw(&models[BENCH_MOD], GLM_COLOR | GLM_SMOOTH);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de la escuela
 ****************************************************************************/
void dibujaEscuela(){
    glClear(GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    
    glTranslated(posXSchool,posYSchool, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(140,180, 0.1);
    glRotatef(-25, 0, 1, 0);

    glmDraw(&models[SCHOOL_MOD], GLM_COLOR  | GLM_SMOOTH);

    glPopMatrix();

}


/****************************************************************************
 * Dibuja modelo de arma (el de arriba)
 ****************************************************************************/
void dibujaGunUp(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXGunUp,posYGunUp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(300,300, 1);
    glRotatef(-45, 0, 1, 0);
    glmDraw(&models[GUN_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de arma (el de abajo)
 ****************************************************************************/
void dibujaGunDown(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    posXGunDown = posXGunUp * -1.0;
    glTranslated(posXGunDown,posYGunDown, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(300,300, 1);
    glRotatef(45, 0, 1, 0);
    glmDraw(&models[GUN_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de hombre (personaje del nivel 2)
 ****************************************************************************/
void dibujaMan(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXMan ,posYMan, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(80,80, 1);
    glmDraw(&models[GUY_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de cigarro (el de arriba)
 ****************************************************************************/
void dibujaCigarUp() {
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    posXCigarUp = posXCigarDown * -1.0;
    glTranslated(posXCigarUp,posYCigarUp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(20,20, 0);
    glRotatef(205, 1, 0, 0);
    glmDraw(&models[CIGAR_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de cigarro (el de abajo)
 ****************************************************************************/
void dibujaCigarDown() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXCigarDown,posYCigarDown, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(20,20, 0);
    glRotatef(205, 1, 0, 0);
    glmDraw(&models[CIGAR_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de droga (la de arriba)
 ****************************************************************************/
void dibujaDrugUp() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    posXDrugUp = posXDrugDown * -1.0;
    glTranslated(posXDrugUp,posYDrugUp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(40,40, 0);
    glRotatef(90, 1, 0, 0);
    glmDraw(&models[DRUG_MOD], GLM_SMOOTH  | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de droga (la de abajo)
 ****************************************************************************/
void dibujaDrugDown() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXDrugDown,posYDrugDown, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(40,40, 0);
    glRotatef(90, 1, 0, 0);
    glmDraw(&models[DRUG_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de botella de cerveza (la de arriba)
 ****************************************************************************/
void dibujaBeerUp() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXBeerUp,posYBeerUp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(20,20, 0);
    
    glmDraw(&models[BEER_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de botella de cerveza (la de abajo)
 ****************************************************************************/
void dibujaBeerDown() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    posXBeerDown = posXBeerUp * -1.0;
    glTranslated(posXBeerDown,posYBeerDown, 0);
    glRotatef(0.8, 1, 0, 0);
    
    glScaled(20,20, 0);
    glmDraw(&models[BEER_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de celular (el de arriba)
 ****************************************************************************/
void dibujaPhoneUp() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXPhoneUp,posYPhoneUp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(30,30, 0);
    glRotatef(-90, 1, 0, 0);
    
    glmDraw(&models[PHONE_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de celular (el de abajo)
 ****************************************************************************/
void dibujaPhoneDown() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    posXPhoneDown = posXPhoneUp * -1.0;
    glTranslated(posXPhoneDown,posYPhoneDown, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(30,30, 0);
    glRotatef(-90, 1, 0, 0);
    
    glmDraw(&models[PHONE_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de Drogger (personaje del nivel 3)
 ****************************************************************************/
void dibujaDrogger(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslated(posXDrogger,posYDrogger, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(30,30, 0);
    
    glmDraw(&models[DROGGER_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja modelo de logo de whatsapp
 ****************************************************************************/
void dibujaWhatsapp() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    posXWhatsapp = posXFbDown * -1.0;
    glTranslated(posXWhatsapp,posYWhatsapp, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(25,25, 0.001);
    glRotatef(-67, 0, 1, 0);
    glmDraw(&models[WHATSAPP_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}
/****************************************************************************
 * Dibuja modelo de logo de facebook
 ****************************************************************************/
void dibujaFbDown() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glTranslated(posXFbDown,posYFbDown, 0);
    glRotatef(0.8, 1, 0, 0);
    glScaled(20,20, 0.001);
    glmDraw(&models[FB_MOD], GLM_SMOOTH | GLM_COLOR);
    glPopMatrix();
}

/****************************************************************************
 * Dibuja la cantidad de vidas del jugador
 ****************************************************************************/
void vidas(int num) {
    glPushMatrix();
    glTranslated(-106, -106, 0);
    glRotatef(0.8, 1, 0, 0);
    glScalef(20, 20, 2);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura
    glBindTexture(GL_TEXTURE_2D, texName[3]);
    
    glBegin(GL_QUADS);
    glColor3ub(129,208, 0);
    float offset = -0.5f;
    for (int i=0; i<num; i++) {
        //Asignar la coordenada de textura 0,0 al vertice
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(offset, -1.0f, 0);
        //Asignar la coordenada de textura 1,0 al vertice
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(offset+1.0f, -1.0f, 0);
        //Asignar la coordenada de textura 1,1 al vertice
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(offset+1.0f, 0.3f, 0);
        //Asignar la coordenada de textura 0,1 al vertice
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(offset, 0.3f, 0);
        offset += 1.25;
    }
    glColor3ub(255, 255, 255);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
}

/****************************************************************************
 * Dibuja la escena/menu correspondiente
 ****************************************************************************/
void drawScene(int scene)
{
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(255, 255, 255);
    glTranslated(0, 0, 0);
    glScalef(28, 28, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del menu/escena que se recibe como parametro
    glBindTexture(GL_TEXTURE_2D, texName[scene]);
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-10.0f, -10.0f, 0);
   
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(10.0f, -10.0f,0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(10.0f, 10.0f, 0);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-10.0f, 10.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
    
}

void drawObjetivo() {
    glPushMatrix();
    glTranslated(0, -106, 0);
    glRotatef(0.8, 1, 0, 0);
    glScalef(20, 20, 2);
    
    glBegin(GL_QUADS);
    glColor3ub(255,208, 0);
   
    
        //Asignar la coordenada de textura 0,0 al vertice
        glVertex3f(-1.0f, -1.0f, 0);
        //Asignar la coordenada de textura 1,0 al vertice
        glVertex3f(1.0f, -1.0f, 0);
        //Asignar la coordenada de textura 1,1 al vertice
        glVertex3f(1.0f, 1.0f, 0);
        //Asignar la coordenada de textura 0,1 al vertice
        glVertex3f(-1.0f, 1.0f, 0);
    
    glEnd();
    char displaycadena[200]="";
    string cadena = "";
    
    sprintf(displaycadena, "%s","Objetivo");
    
    draw3dString(GLUT_STROKE_MONO_ROMAN,displaycadena,0.0, 0.0, 2.0);
    
    glColor3ub(255, 255, 255);
    glPopMatrix();
    glutPostRedisplay();
}
void dibujaBackground()
{
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslated(0, 0, 0);
    glRotatef(0.8, 1, 0, 0);
    glScalef(28, 28, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del background
    glBindTexture(GL_TEXTURE_2D, texName[14]);
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-10.0f, -10.0f, 0);
    
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(10.0f, -10.0f,0);
    
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(10.0f, 10.0f, 0);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-10.0f, 10.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
    
}

/****************************************************************************
 * Reinicia el juego
 ****************************************************************************/
void reinicio(){
    actualTexture = LEVEL1;
    vidastotal = 3;
   
    // Posiciones de jugador y metas del nivel
    posXPersonaje = 0.0, posYPersonaje = -85.0;
    posXBusStop = 0.0, posYBusStop = 120.0;
    posXSemaforo = 0.0, posYSemaforo = 120.0;
    posXSchool = 0.0, posYSchool = 130.0;
    
    
    // Obstaculos Nivel 1
    // Determinan la direccion del movimiento
    beerAscending = true;
    drugAscending = true;
    cigarAscending = true;
    
    // Posiciones obstaculos nivel 1
    posXCigarUp = 0, posYCigarUp = -55.0;
    posXCigarDown = 0, posYCigarDown = -75.0;
    posXBeerUp = -140.0, posYBeerUp = 0.0;
    posXBeerDown = 140.0, posYBeerDown = -20.0;
    posXDrugUp = 130, posYDrugUp = 60;
    posXDrugDown = -130, posYDrugDown = 40;
    
    // Obstaculos Nivel 2
    // Determinan la direccion del movimiento
    gunAscending = true;
    manAscending = true;
    // Posiciones obstaculos nivel 1
    posXGunUp = -130.0, posYGunUp = -15.0;
    posXGunDown = 130.0, posYGunDown = -35.0;
    posXMan = 0.0, posYMan = -60.0;
    
    // Obstaculos Nivel 3
    // Determinan la direccion del movimiento
    fbAscending = true;
    phoneAscending = true;
    droggerAscending = true;
    // Posiciones obstaculos nivel 1
    posXPhoneUp = -130.0, posYPhoneUp = -45.0;
    posXPhoneDown = 130.0, posYPhoneDown = -70.0;
    posXDrogger = 0.0, posYDrogger = -15.0;
    posXWhatsapp = 130.0, posYWhatsapp = 20.0;
    posXFbDown = -130.0, posYFbDown = 0.0;
    
    finished = false;
    start = false;
    play = true;
    running = false;
}

/****************************************************************************
 * Inicializa variables y modelos
 ****************************************************************************/
void init(){
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    running = false;
    enteroGlobal = 0;
    
    finished = false;
    start = false;
    
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT );
    
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    
    //Drogger
    std::string ruta = fullPath + "objects/drogger1.obj";
    models[DROGGER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[DROGGER_MOD]);
    glmVertexNormals(&models[DROGGER_MOD], 90.0, GL_TRUE);
    
    // Personaje
    ruta = fullPath + "objects/kid5.obj";
    models[PLAYER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PLAYER_MOD]);
    glmVertexNormals(&models[PLAYER_MOD], 90.0, GL_TRUE);
    
    //busstop
    ruta = fullPath + "objects/busstop2.obj";
    models[BUSSTOP_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[BUSSTOP_MOD]);
    glmVertexNormals(&models[BUSSTOP_MOD], 90.0, GL_TRUE);
    
    //beer
    ruta = fullPath + "objects/Beer.obj";
    models[BEER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[BEER_MOD]);
    glmVertexNormals(&models[BEER_MOD], 90.0, GL_TRUE);
    
    //Droga
    ruta = fullPath + "objects/weed.obj";
    models[DRUG_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[DRUG_MOD]);
    glmVertexNormals(&models[DRUG_MOD], 90.0, GL_TRUE);
    
    //Pistola
    ruta = fullPath + "objects/gun.obj";
    models[GUN_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[GUN_MOD]);
    glmVertexNormals(&models[GUN_MOD], 90.0, GL_TRUE);
    
    // Cigarros
    ruta = fullPath + "objects/cigar.obj";
    models[CIGAR_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[CIGAR_MOD]);
    glmVertexNormals(&models[CIGAR_MOD], 90.0, GL_TRUE);
    
    // Semaforo
    ruta = fullPath + "objects/light1.obj";
    models[SEMAFORO_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[SEMAFORO_MOD]);
    glmVertexNormals(&models[SEMAFORO_MOD], 90.0, GL_TRUE);
    
    // Hombre nivel 2
    ruta = fullPath + "objects/guy2.obj";
    models[GUY_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[GUY_MOD]);
    glmVertexNormals(&models[GUY_MOD], 90.0, GL_TRUE);
    
    // Columpios parque
    ruta = fullPath + "objects/playground.obj";
    models[PLAYSET_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PLAYSET_MOD]);
    glmVertexNormals(&models[PLAYSET_MOD], 90.0, GL_TRUE);
    
    // Banca parque
    ruta = fullPath + "objects/bench.obj";
    models[BENCH_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[BENCH_MOD]);
    glmVertexNormals(&models[BENCH_MOD], 90.0, GL_TRUE);
    
    // Escuela
    ruta = fullPath + "objects/school.obj";
    models[SCHOOL_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[SCHOOL_MOD]);
    glmVertexNormals(&models[SCHOOL_MOD], 90.0, GL_TRUE);
    
    // Celular
    ruta = fullPath + "objects/phone1.obj";
    models[PHONE_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PHONE_MOD]);
    glmVertexNormals(&models[PHONE_MOD], 90.0, GL_TRUE);
    
    // Logo de facebook
    ruta = fullPath + "objects/fb8.obj";
    models[FB_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[FB_MOD]);
    glmVertexNormals(&models[FB_MOD], 90.0, GL_TRUE);
    
    // Logo de whatsapp
    ruta = fullPath + "objects/Whatsapp.obj";
    models[WHATSAPP_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[WHATSAPP_MOD]);
    glmVertexNormals(&models[WHATSAPP_MOD], 90.0, GL_TRUE);
    
}



//********* Eventos del teclado ***********//
//*****************************************//
void myKeyboard(unsigned char theKey, int x, int y){
    
    switch (theKey){
        case 'j': // Si presiona J o j
        case 'J':
            if (actualTexture == MAINMENU_TEXTURE) { // Si se encuentra en el menu principal
                play = true;
                Mix_HaltMusic();
                actualTexture = LEVEL1; // Inicia nivel 1
            } else if (actualTexture == WINLEVEL1_TEXTURE) { // Si gana el nivel 1
                play = true;
                actualTexture = LEVEL2; // Inicia nivel 2
                Mix_PlayMusic( gMusic, 0 );
            } else if (actualTexture == WINLEVEL2_TEXTURE) { // Si gana el nivel 2
                play = true;
                actualTexture = LEVEL3; // Inicial nivel 3
                Mix_PlayMusic( gMusic, 0 );
            } else if (actualTexture == GAMEOVER_TEXTURE) { // Si pierde
                play = true;
                start = false;
                actualTexture = LEVEL1; // Vuelve a jugar nivel 1
                Mix_PlayMusic( gMusic, 0 );
            } else if (actualTexture == COMPLETEGAME_TEXTURE) { // Si completa el juego
                play = true;
                start = false;
                Mix_PlayMusic( gMusic, 0 );
                reinicio(); // Vuelve a jugar nivel 1
            }
            
            break;
        case 's': // Si presiona S o s
        case 'S':
            if (actualTexture == MAINMENU_TEXTURE) { // Si se encuentra en el menu principal
                exit(0); // Sale del juego
            } else {
                if (play && start) {
                    if (posYPersonaje-10.0>=-85) {
                        posYPersonaje -= 10.0;
                    }
                }
            }
            
            break;
        case 'I':
        case 'i':
            if (!play && actualTexture == MAINMENU_TEXTURE) {
                actualTexture = INSTRUCTIONS_TEXTURE;
            } else if (play && (actualTexture == LEVEL1 || actualTexture == LEVEL2 || actualTexture == LEVEL3)){
                if (!finished){
                    Mix_PlayMusic( gMusic, 0 );
                    start = true;
                    running = true;
                }
            }
            
            break;
        case 'C':
        case 'c':
            if (!play && actualTexture == MAINMENU_TEXTURE) {
                actualTexture = CREDITS_TEXTURE;
            }
            break;
        case 'P':
        case 'p':
            if (play && (actualTexture == LEVEL1 || actualTexture == LEVEL2 || actualTexture == LEVEL3)){
                Mix_HaltMusic();
                start = false;
                running = false;
            }
            
            break;
            
        case 'R':
        case 'r':
            if (actualTexture == INSTRUCTIONS_TEXTURE || actualTexture == CREDITS_TEXTURE) {
                actualTexture = MAINMENU_TEXTURE;
            } else if (actualTexture == GAMEOVER_TEXTURE || actualTexture == COMPLETEGAME_TEXTURE) {
                play = false;
                start = false;
                 Mix_PlayMusic( gMusic, 0 );
                actualTexture = MAINMENU_TEXTURE;
                vidastotal = 3;
            } else if (actualTexture == LEVEL1 || actualTexture == LEVEL2 || actualTexture == LEVEL3) {
                // reset
                reinicio();
            }
            break;
        case 'w':
        case 'W':
            if (play && start){
                if (posYPersonaje+10.0 <= 120){
                    posYPersonaje += 10.0;
                }
            }
            break;
        case 'a':
        case 'A':
            if (play && start){
                if (posXPersonaje-10>=-140){
                    posXPersonaje -= 10.0;
                }
            }
            
            break;
        case 'd':
        case 'D':
            if (play && start) {
                if (posXPersonaje+10<=140) {
                    posXPersonaje += 10.0;
                }
            }
            break;
        case 27:
            exit(0);
            break;
    }
}
//********* Eventos del teclado especial ******
//********************************************//
void mySpecialKeyboard(int key, int x, int y) {
    if (start) {
        switch (key) {
            case GLUT_KEY_F1:
                if (posYPersonaje+10.0 <= 120) {
                    posYPersonaje += 10.0;
                }
                
                
                break;
            case GLUT_KEY_DOWN:
                if (posYPersonaje-10.0>=-85) {
                    posYPersonaje -= 10.0;
                }
                
                break;
            case GLUT_KEY_LEFT:
                if (posXPersonaje-10>=-140){
                    posXPersonaje -= 10.0;
                }
                
                break;
            case GLUT_KEY_RIGHT:
                if (posXPersonaje+10<=140) {
                    posXPersonaje += 10.0;
                }
                
                break;
                
            default:
                break;
        }
    }
    
    
}
/****************************************************************************
 * Timer del juego - habilita movimientos y colisiones
 ****************************************************************************/
void timer(int value)
{
    if (running) {
        
        if (variableLightPosition < 5.0){
            variableLightPosition+= 0.2;
        } else {
            variableLightPosition = 0.0;
        }
        angulo = (angulo + 1) % 360;
        if (start && actualTexture == LEVEL1){
            movimientoBeer();
            movimientoDrug();
            movimientoCigar();
            colisionCigar();
            colisionBeer();
            colisionDrug();
            colisionBusStop();
        } else if (start && actualTexture == LEVEL2){
            colisionSemaforo();
            movimientoGun();
            colisionGun();
            movimientoMan();
            colisionMan();
        } else if (start && actualTexture == LEVEL3){
            
            movimientoPhone();
            movimientoDrogger();
            movimientoFb();
            colisionSchool();
            colisionDrogger();
            colisionPhone();
            colisionFb();
            colisionWhatsapp();
        }
        if (vidastotal == 0) {
            play = false;
            actualTexture = GAMEOVER_TEXTURE;
            vidastotal = 3;
            Mix_HaltMusic();
            Mix_PlayChannel( 0, gameFail, 0 );
        }
        
        
        
    }
    
    
    glutPostRedisplay();
    glutTimerFunc(3, timer, 1); //el ultimo parametro es para control
    
}

/****************************************************************************
 * Dibuja la escena y modelos correspondientes al nivel
 ****************************************************************************/
void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!play) {
        drawScene(actualTexture);
    } else if (play && actualTexture == LEVEL1){
        
        
        glPushMatrix();
        glRotatef(-0.8, 1, 0, 0);
        
        dibujaBackground();
        
        dibujaCesped(-90.0,10.0,0.0,300.0);
        dibujaCalle(-56.0, -0.2, 0.0,300.0,50.0);
        dibujaCesped(-11.0,30.0,0.0,300.0);
        dibujaAreaSegura(8.0,1.0,10.0,193.0);
        dibujaAreaSegura(65.0,1.0,20.0, 193.0);
        dibujaCalle(35.0,-0.4, 1.0,193.0,40.0);
        
        dibujaCigarUp();
        dibujaCigarDown();
        dibujaDrugUp();
        dibujaDrugDown();
        dibujaBeerUp();
        dibujaBeerDown();

        dibujaBusStop();

        dibujaPersonaje(posXPersonaje, posYPersonaje);

        vidas(vidastotal);
       // drawObjetivo();
        glPopMatrix();
        
    } else if (play && actualTexture == LEVEL2) {
        
        glPushMatrix();
        glRotatef(-0.8, 1, 0, 0);
        
        dibujaBackground();
        
        dibujaAreaSegura(-90.0,0.0,11.0,300.0);
        dibujaCesped(0.0,90.0,0.0,300.0);
        dibujaAreaSegura(10.0,1.0, 10.0,193.0);
        dibujaCalle(44, 0, 1.0 ,193.0 ,50.0);
        dibujaAreaSegura(35.0,2.0, 10.0,86.0);
        
        dibujaPlayset();
        dibujaBench();
        dibujaGunUp();
        dibujaGunDown();
        dibujaMan();

        dibujaSemaforo();

        dibujaPersonaje(posXPersonaje, posYPersonaje);
        vidas(vidastotal);
        glPopMatrix();
    } else if (play && actualTexture == LEVEL3){
        
        
        glPushMatrix();
        glRotatef(-0.8, 1, 0, 0);
        
        dibujaBackground();
        
        dibujaAreaSegura(-87.0,0.1,10.0,290.0);
        dibujaCalle(-57, 0, 0.0,300.0,50.0);
        dibujaAreaSegura(-19,1.0, 10.0,193.0);
        dibujaCalle(2, 0,1.0,193.0,30.0);
        dibujaCesped(61, 50.0,1.0,193.0);
        
        dibujaPhoneUp();
        dibujaPhoneDown();
        
        dibujaWhatsapp();
        dibujaFbDown();
        dibujaDrogger();

        dibujaEscuela();

        
        dibujaPersonaje(posXPersonaje, posYPersonaje);
        
        vidas(vidastotal);
        glPopMatrix();
    }

    glutSwapBuffers();
    
}

/****************************************************************************
 * Acciones del menu de opciones
 ****************************************************************************/
void myMenu(int entryID)
{
   
    
    switch (entryID) {
        case 1:
            start = true;
            running = true;
            break;
        case 2:
            reinicio();
            break;
        case 3:
            start = false;
            running = false;
            break;
        case 4:
            reinicio();
            play = false;
            start = false;
            actualTexture = MAINMENU_TEXTURE;
            break;
        case 5:
            exit(0);
        default:
            break;
    }
    
    // volver a dibujar
    glutPostRedisplay();
}

/****************************************************************************
 * Menu de Opciones - Click derecho
 ****************************************************************************/
void createMenus(){
    
    // crear el menú e indicar la función callback
    glutCreateMenu(myMenu);
    
    // agregar entradas
    glutAddMenuEntry("1. Iniciar",1);
    glutAddMenuEntry("2. Reiniciar",2);
    glutAddMenuEntry("3. Pausar",3);
    glutAddMenuEntry("4. Regresar al Menú",4);
    glutAddMenuEntry("5. Salir",5);

    // atar el menú al botón derecho
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;
    
    //Load music
    char rutamusica[200] = "";
    
    sprintf(rutamusica,"%s%s", fullPath.c_str() , "sounds/background.wav");
    gMusic = Mix_LoadMUS(rutamusica);
    if( gMusic == NULL )
    {
        printf( "Failed to load gmusic music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    //Load sound effects
    sprintf(rutamusica,"%s%s", fullPath.c_str() , "sounds/gamefail.wav");
    gameFail = Mix_LoadWAV(rutamusica);
    if( gameFail == NULL )
    {
        printf( "Failed to load gamefail sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    sprintf(rutamusica,"%s%s", fullPath.c_str() , "sounds/levelcomplete.wav");
    levelcomplete = Mix_LoadWAV(rutamusica);
    if( levelcomplete == NULL )
    {
        printf( "Failed to load levelcomplete sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    sprintf(rutamusica,"%s%s", fullPath.c_str() , "sounds/gamewin.wav");
    gamewin = Mix_LoadWAV(rutamusica);
    if( gamewin == NULL )
    {
        printf( "Failed to load gamewin sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    sprintf(rutamusica,"%s%s", fullPath.c_str() , "sounds/pierdevida.wav");
    pierdevida = Mix_LoadWAV(rutamusica);
    if( pierdevida == NULL )
    {
        printf( "Failed to load pierdevida sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }

    Mix_PlayMusic( gMusic, 0 );
    
    return success;
}

void close()
{
    
    //Free the sound effects
    Mix_FreeChunk( gamewin );
    Mix_FreeChunk( gameFail );
    Mix_FreeChunk( pierdevida );
    Mix_FreeChunk( levelcomplete );
    gamewin = NULL;
    gameFail = NULL;
    pierdevida = NULL;
    levelcomplete = NULL;
    
    //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;
    

    
    //Quit SDL subsystems
    Mix_Quit();
    SDL_Quit();
}




int main(int argc, char *argv[])
{
    //Find root path for files
    getParentPath();
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth,screenHeight);
    glutInitWindowPosition(10,10);
    //initialise openAL
    //alutInit(&argc, argv) ;
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Drogger");
    initRendering();
    init();
    loadMedia();
    //close();
    glutTimerFunc(100, timer, 1);
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeyboard);
    createMenus();
    glutReshapeFunc(reshape);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}



