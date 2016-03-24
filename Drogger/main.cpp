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

#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>

#include "glm.h"

//Amount of models and model ids
#define MODEL_COUNT 10
#define PLAYER_MOD 0

using namespace std;

/////////////
string fullPath = __FILE__;

GLMmodel models[MODEL_COUNT];

int	screenWidth = 700, screenHeight = 700;
double tableroWidth=100.0, tableroHeigth=100.0;
double i = 0;
int angulo = 0;
int enteroGlobal = 0;
bool running = false;



bool finished = false;
bool start = false;


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
    
    angulo = (angulo + 10) % 360;
    glutPostRedisplay();
    glutTimerFunc(100, timer, 1); //el ultimo parametro es para control
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
    glutSolidCube(1);
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
    glutSolidCube(1);
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
    glutSolidCube(1);
    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3f(0.0f, rayasY, 0.0f);
    glVertex3f(-0.35f, rayasY,2.0f);
    glVertex3f(-0.25f, rayasY,2.0f);
    
    glVertex3f(-0.15f, rayasY,2.0f);
    glVertex3f(-0.05f, rayasY,2.0f);
    
    glVertex3f(0.15f, rayasY,2.0f);
    glVertex3f(0.05f, rayasY,2.0f);
    
    glVertex3f(0.25f, rayasY,2.0f);
    glVertex3f(0.35f, rayasY,2.0f);
    
    glVertex3f(-0.35f, rayasY+0.3f,2.0f);
    glVertex3f(-0.25f, rayasY+0.3f,2.0f);
    
    glVertex3f(-0.15f, rayasY+0.3f,2.0f);
    glVertex3f(-0.05f, rayasY+0.3f,2.0f);
    
    glVertex3f(0.15f, rayasY+0.3f,2.0f);
    glVertex3f(0.05f, rayasY+0.3f,2.0f);
    
    glVertex3f(0.25f, rayasY+0.3f,2.0f);
    glVertex3f(0.35f, rayasY+0.3f,2.0f);
    glEnd();
    glPopMatrix();

    glutPostRedisplay();
}

void dibujaPersonaje() {
    glPushMatrix();
    glTranslated(5, 5, 0);
    glRotated(angulo, 0, 1, 0);
    glScaled(50, 50, 5);
    glmDraw(&models[PLAYER_MOD], GLM_COLOR | GLM_SMOOTH);
    glPopMatrix();
}
void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef(-0.4, 1, 0, 0);
    dibujaCesped(-100.0,20.0);
    dibujaCesped(-10.0,50.0);
    dibujaCalle(-60.0, 0.0);
    dibujaAreaSegura(25.0);
    dibujaCalle(62.0,-0.3);
    dibujaAreaSegura(100.0);
    glPopMatrix();
    //dibujaPersonaje();
    glutSwapBuffers();
    
}

void init(){
    running = false;
    enteroGlobal = 0;

    finished = false;
    start = false;
    
    
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT );
    
    //player
    std::string ruta = fullPath + "objects/Patrick.obj";
    std::cout << "Filepath: " << ruta << std::endl;
    models[PLAYER_MOD] = *glmReadOBJ(ruta.c_str());
    glmUnitize(&models[PLAYER_MOD]);
    glmVertexNormals(&models[PLAYER_MOD], 90.0, GL_TRUE);
    
    
}

void getParentPath()
{
    for (int i = (int)fullPath.length()-1; i>=0 && fullPath[i] != '/'; i--) {
        fullPath.erase(i,1);
    }
}

//********* Eventos del teclado ***********//
//*****************************************//
void myKeyboard(unsigned char theKey, int x, int y){
    
}
//********* Eventos del teclado especial ******
//********************************************//
void mySpecialKeyboard(int key, int x, int y) {

    
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
    init();
    glutTimerFunc(100, timer, 1); //el ultimo parametro es para control
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeyboard);
    
    glutReshapeFunc(reshape);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}
