//
//  main.cpp
//  Drogger
//
//  Created by Omar Carreon and Ricardo Perez on 15/03/16.
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

//Amount of models and model ids
#define MODEL_COUNT 10
#define DROGGER_MOD 0
#define PLAYER_MOD 1
#define BUSSTOP_MOD 2

/////////////
string fullPath = __FILE__;
using namespace std;

static GLuint texName[36];
const int TEXTURE_COUNT=7;

GLMmodel models[MODEL_COUNT];

int	screenWidth = 700, screenHeight = 700;
double tableroWidth=100.0, tableroHeigth=100.0;
double i = 0;
int angulo = 0;
int enteroGlobal = 0;
bool running = false;
double posXPersonaje = 0, posYPersonaje = -130.0;


bool finished = false;
bool start = false;

//le borramos el exceso para solo obtener el Path padre
void getParentPath()
{
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i,1);
    }
}

void draw3dString (void *font, char *s, float x, float y, float z)
{
    unsigned int i;
    glMatrixMode(GL_MODELVIEW);
    glLineWidth(1);
    glPushMatrix();
    glTranslatef(x, y, z);
    
    glScaled(0.005, 0.005, 1);
    
    for (i = 0; i < s[i] != '\0'; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, s[i]);
                 
    }
    glPopMatrix();
}

void timer(int value)
{
    if (running) {
        angulo = (angulo + 1) % 360;
        cout << angulo << endl;
    }
        glutPostRedisplay();
        glutTimerFunc(100, timer, 1); //el ultimo parametro es para control
    
    
}

//Makes the image into a texture, and returns the id of the texture
void loadTexture(Image* image,int k)
{
    
    glBindTexture(GL_TEXTURE_2D, texName[k]); //Tell OpenGL which texture to edit
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //Filtros de ampliacion y redución con cálculo mas cercano no es tan bueno pero es rápido
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
    //Filtros de ampliacion y redución con cálculo lineal es mejo pero son más calculos
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
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
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/menu.bmp");
    LoadGLTextures(ruta,i++);
    
    sprintf(ruta,"%s%s", fullPath.c_str() , "textures/instrucciones.bmp");
    LoadGLTextures(ruta,i++);

    
    delete image;
}


void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-tableroWidth, tableroWidth, -tableroHeigth, tableroHeigth, 1.0, 10.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 1.8, 0, 0, 0, 0, 5, 10);
    
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



void dibujaCesped(double y,double height){
    glPushMatrix();
    glColor3ub(0,255, 0);
    glTranslated(0, y, 0);
    glRotatef(0, 1.0, 0.0, 0.0);
    glRotatef(0, 0.0, 1.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glScalef(300, height, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    
    glBegin(GL_QUADS);
    
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.3f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.3f, 0);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutPostRedisplay();
}

void dibujaAreaSegura(double y) {
    glPushMatrix();
    glColor3ub(255,255,224);
    glTranslated(0, y, 0);
    glRotatef(0, 1.0, 0.0, 0.0);
    glRotatef(0, 0.0, 1.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glScalef(300, 20.0, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[2]);
    glBegin(GL_QUADS);
    
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(3.0f, 0.0f);
    glVertex3f(0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(3.0f, 1.0f);
    glVertex3f(0.5f, 0.3f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.3f, 0);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
}

void dibujaCalle(double y, double rayasY) {
    glPushMatrix();
    glColor3ub(128,128, 128);
    glTranslated(0, y, 0);
    glRotatef(0, 1.0, 0.0, 0.0);
    glRotatef(0, 0.0, 1.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glScalef(300, 50.0, 0.1);

    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[1]);
    
    glBegin(GL_QUADS);
    float roadw = -0.5f;
    for (int i=0;i<4; i++) {
        
        //Asignar la coordenada de textura 0,0 al vertice
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(roadw, -0.62f, 0);
        //Asignar la coordenada de textura 1,0 al vertice
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(roadw+0.25, -0.62f, 0);
        //Asignar la coordenada de textura 1,1 al vertice
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(roadw+0.25, 0.3f, 0);
        //Asignar la coordenada de textura 0,1 al vertice
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(roadw, 0.3f, 0);
        roadw += 0.25;
    }
    
    glEnd();
    
    
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glutPostRedisplay();
}

void dibujaPersonaje(double posX, double posY) {
    posXPersonaje = posX;
    posYPersonaje = posY;
    glClear(GL_DEPTH_BUFFER_BIT); // Se usa para limpiar buffer depth (que los modelos se pongan enfrente de todo)
    glPushMatrix();
    glTranslated(posXPersonaje, posYPersonaje, 0);
    glScaled(30, 30, 0);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR | GLM_SMOOTH);
    glPopMatrix();
}

// ARREGLAR BUSSTOP
void dibujaBusStop() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    glScaled(20,10, 2);
    glRotated(-80, 1, 0, 0);
    glTranslated(0,0, 4.8);
    glmDraw(&models[BUSSTOP_MOD], GLM_COLOR | GLM_SMOOTH);
    glPopMatrix();
}

void vidas() {
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslated(0, 200, 0);
    glScalef(80, 80, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[3]);
    
    glBegin(GL_QUADS);
    glColor3ub(255,255, 255);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -1.0f, 0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.3f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.3f, 0);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
}

void drawScene(int scene)
{
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslated(0, 0, 0);
    glScalef(18, 18, 0.1);
    
    //Habilitar el uso de texturas
    glEnable(GL_TEXTURE_2D);
    
    //Elegir la textura del Quads: angulo cambia con el timer
    glBindTexture(GL_TEXTURE_2D, texName[scene]);
    
    glBegin(GL_QUADS);
    //Asignar la coordenada de textura 0,0 al vertice
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-10.0f, -10.0f, 0);
    //Asignar la coordenada de textura 1,0 al vertice
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(10.0f, -10.0f,0);
    //Asignar la coordenada de textura 1,1 al vertice
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(10.0f, 10.0f, 0);
    //Asignar la coordenada de textura 0,1 al vertice
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-10.0f, 10.0f, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
    glutPostRedisplay();
    
}
void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //vidas();
    drawScene(5);
    glPushMatrix();
    
    glRotatef(-0.4, 1, 0, 0);
    /*
    dibujaCesped(-90.0,10.0);
    dibujaCalle(-56.0, -0.2);
    dibujaCesped(-11.0,30.0);
    dibujaAreaSegura(15.0);
    dibujaAreaSegura(85.0);
    dibujaCalle(50.0,-0.4);
    */
    glPopMatrix();
    //dibujaPersonaje(posXPersonaje, posYPersonaje);
    //dibujaBusStop();
    
    glutSwapBuffers();
    
}

void init(){
    
    
    running = false;
    enteroGlobal = 0;

    finished = false;
    start = false;
    
    
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT );
    
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    
    //Drogger
    std::string ruta = fullPath + "objects/Patrick.obj";
    models[DROGGER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[DROGGER_MOD]);
    glmVertexNormals(&models[DROGGER_MOD], 90.0, GL_TRUE);
    
    // Personaje
    ruta = fullPath + "objects/Kid.obj";
    models[PLAYER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PLAYER_MOD]);
    glmVertexNormals(&models[PLAYER_MOD], 90.0, GL_TRUE);
    
    //busstop
    ruta = fullPath + "objects/bustop.obj";
    models[BUSSTOP_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[BUSSTOP_MOD]);
    glmVertexNormals(&models[BUSSTOP_MOD], 90.0, GL_TRUE);
}



//********* Eventos del teclado ***********//
//*****************************************//
void myKeyboard(unsigned char theKey, int x, int y){
    
    switch (theKey){
        case 'w':
        case 'W':
            if (running) {

            }
            break;
        case 's':
        case 'S':
            if (running){
            }
            break;
        case 'I':
        case 'i':
            if (!finished){
                start = true;
                running = true;
            }
            break;
        case 'P':
        case 'p':
            start = false;
            running = false;
            break;
            
        case 'R':
        case 'r':
            // reset
            init();
            break;
            
        case 27:
            exit(0);
            break;
    }
}
//********* Eventos del teclado especial ******
//********************************************//
void mySpecialKeyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if (posYPersonaje+10.0 <= 90) {
                posYPersonaje += 10.0;
            }
            
            cout << posYPersonaje << endl;
            
            break;
        case GLUT_KEY_DOWN:
            if (posYPersonaje-10.0>=-130) {
                posYPersonaje -= 10.0;
            }
            
            break;
        case GLUT_KEY_LEFT:
            if (posXPersonaje-10>=-160){
                posXPersonaje -= 10.0;
            }
            
            cout << posXPersonaje << endl;
            break;
        case GLUT_KEY_RIGHT:
            if (posXPersonaje+10<=160) {
                posXPersonaje += 10.0;
            }
            
            break;
            
        default:
            break;
    }
    
}



int main(int argc, char *argv[])
{
    //Find root path for files
    getParentPath();
    glutInit(&argc, argv);
    glutInitWindowSize(screenWidth,screenHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Drogger");
    initRendering();
    init();
    glutTimerFunc(100, timer, 1); //el ultimo parametro es para control
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeyboard);
    
    glutReshapeFunc(reshape);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}



