#include <GL/glew.h>
#include <glut/glut.h>
#include <iostream>

GLuint Truck;


float White[] = { 1.,1.,1.,1. };



float *
Array3(float a, float b, float c)
{
    static float array[4];
    array[0] = a;
    array[1] = b;
    array[2] = c;
    array[3] = 1.;
    return array;
}

// utility to create an array from a multiplier and an array:
float *
MulArray3(float factor, float array0[3])
{
    static float array[4];
    array[0] = factor * array0[0];
    array[1] = factor * array0[1];
    array[2] = factor * array0[2];
    array[3] = 1.;
    return array;
}



void
SetMaterial(float r, float g, float b, float shininess)
{
    glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
    glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
    glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
    glMaterialf(GL_BACK, GL_SHININESS, 1.f);
    glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
    glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


void DrawCylinder(float thickness, float radius, float length) {
    
    float x = 0;
    float y = 0;
    float angle = 0.0;
    float angle_stepsize = 0.1;
    
    // float radius = 0.05;
    // float height = 1.5;  // thickness of tube
    float pi = 3.1415025;
    
    angle = 0.0;
    while (angle < 2 * pi) {
        
        x = radius * cos(angle);
        y = radius * sin(angle);
        
        glVertex3f(x, y, thickness / length);
        glVertex3f(x, y, 0);
        angle = angle + angle_stepsize;
        
    }
    
    glVertex3f(radius, 0.0, thickness / length);
    glVertex3f(radius, 0.0, 0);
    
    
    
}



void truck(){
    // upper truck
    Truck = glGenLists(1);
    glNewList(Truck, GL_COMPILE);
    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    DrawCylinder(2, 0.09, 1.5);
    glEnd();
    glPopMatrix();
    glEndList();
    
}