////////////////////////////////////////////////////////////////
// School of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
//
// Skeleton code for COMP37111 coursework, 2013-14
//
// Authors: Arturs Bekasovs and Toby Howard
//
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "SOIL.h"

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.14159
#define STAR_NO 1000
#define MASS_PART_NO 1
#define G 1.0
#define PART_MASS_RANGE 1000
#define MAX_CAMERA 1000000

// Display list for coordinate axis 
GLuint axisList;

GLint AXIS_SIZE= 500;
int renderMode = 3;
int star_radius = 2000000;
int current_time = 0;
bool pause = true;
double TIME  = 0.3;
bool perspective = false;
int PART_NO = 16384;
int newPartNo = 16384;
GLuint fuzzyTexture;

float cameraX;
float cameraY;
float cameraZ;

int cameraLat = 90;
int cameraLon = 0;
int cameraRadius = 90000;

GLfloat star_coods[STAR_NO][3];

typedef struct { 
GLfloat r, g, b; //color
GLfloat x, y, z; //position
GLfloat u, v, w; //velocity vector
GLfloat Au, Av, Aw; //accel vector
GLfloat mass; //mass of the particle
bool alive;
} particle;

particle* particles;
particle mass_paritcles[MASS_PART_NO];

float myRand (void)
{
/* return a random float in the range [0,1] */

  return ((float) rand() / RAND_MAX);
}

float distance(float xa, float ya, float za, float xb, float yb, float zb) {
  return sqrt(pow(xb-xa, 2.0) + pow(yb-ya, 2.0) + pow(zb-za, 2.0));
}


void calcParticleValues() {
  int i;
  int j;
  float v;
  float force;
  float r;
  float diffX;
  float diffY;
  float diffZ;
  for(i = 0; i < PART_NO; i++) {
    if(particles[i].alive) {
//calc abs value for current velocity
      v = sqrt(pow(particles[i].u, 2.0) + pow(particles[i].v, 2.0) + pow(particles[i].w, 2.0));

//calculate new position according to velocity
      particles[i].x += particles[i].u * TIME;
      particles[i].y += particles[i].v * TIME;
      particles[i].z += particles[i].w * TIME;

      particles[i].Au = 0;
      particles[i].Av = 0;
      particles[i].Aw = 0;

      for(j = 0; j < MASS_PART_NO; j++) {

//check if particle is too close to mass(is destoryed if so)
//approximated to a cube
        if(distance(mass_paritcles[j].x, mass_paritcles[j].y, mass_paritcles[j].z, particles[i].x, particles[i].y, particles[i].z) < log(mass_paritcles[j].mass) * 400) {
          particles[i].alive = false;
          continue;
      }

//calculate distance between the objects
      diffX = mass_paritcles[j].x - particles[i].x;
      diffY = mass_paritcles[j].y - particles[i].y;
      diffZ = mass_paritcles[j].z - particles[i].z;

      r = sqrt(pow(diffX, 2.0) + pow(diffY, 2.0) + pow(diffZ, 2.0));

      force = (G * mass_paritcles[j].mass * particles[i].mass) / r;
      particles[i].Au += (diffX / r * force) / particles[i].mass;
      particles[i].Av += (diffY / r * force) / particles[i].mass;
      particles[i].Aw += (diffZ / r * force) / particles[i].mass;
    }

    particles[i].g = v / 1000.0;
    particles[i].b = v / 1000.0;

//calculate new velocity according to acceleration (force)
//take off some velocity to decay orbits
    //heavier objects decay a bit more
    particles[i].u += particles[i].Au * particles[i].mass/(PART_MASS_RANGE*4) * TIME;
    particles[i].v += particles[i].Av * particles[i].mass/(PART_MASS_RANGE*4) * TIME;
    particles[i].w += particles[i].Aw * particles[i].mass/(PART_MASS_RANGE*4) * TIME;
  }
}
}

void drawParticles() {

  if(renderMode == 1) {
    //glBindTexture(GL_TEXTURE_2D, fuzzyTexture);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
  }

//draw particles
  int i;

  for(i = 0; i < PART_NO; i++) {
    if(particles[i].alive) {
      if(renderMode == 1) {
        if(perspective)
          glPointSize(6000000.0/(distance(cameraX, cameraY, cameraZ, particles[i].x, particles[i].y, particles[i].z)*5));
        else
          glPointSize(20.0);
        glBegin(GL_POINTS);
        glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glVertex3f(particles[i].x, particles[i].y, particles[i].z);
        glEnd();
      }
      else if(renderMode == 2) {
        glLineWidth(1.0);
        glBegin(GL_LINES);
        glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glVertex3f(particles[i].x - particles[i].u*2, particles[i].y - particles[i].v*2, particles[i].z - particles[i].w*2);
        glVertex3f(particles[i].x + particles[i].u*2, particles[i].y + particles[i].v*2, particles[i].z + particles[i].w*2);
        glEnd();

      }
      else {
        if(perspective)
          glPointSize(400000.0/distance(cameraX, cameraY, cameraZ, particles[i].x, particles[i].y, particles[i].z));
        else
          glPointSize(2.0);
        glBegin(GL_POINTS);
        glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glVertex3f(particles[i].x, particles[i].y, particles[i].z);
        glEnd();
      }
    }
  }

//draw massive particles
  if(renderMode == 1)
    glPointSize(20.0);
  else
    glPointSize(5.0);

  glBegin(GL_POINTS);

  for(i = 0; i < MASS_PART_NO; i++) {
    glColor3f(mass_paritcles[i].r, mass_paritcles[i].g, mass_paritcles[i].b);
    glVertex3f(mass_paritcles[i].x, mass_paritcles[i].y, mass_paritcles[i].z);
  }
  glEnd();

  if(renderMode == 1) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
  }
}

void randPointOnSphere(float Cx, float Cy, float Cz, float r, float* x, float* y, float* z) {
  float inclination;
  float azimuth;

  inclination = myRand() * PI;
  azimuth = myRand() * 2 * PI;

  *x = r * sin(inclination) * cos(azimuth) + Cx; 
  *y = r * sin(inclination) * sin(azimuth) + Cy;
  *z = r * cos(inclination) + Cz;
}

void randPointInSphere(float Cx, float Cy, float Cz, float r, float* x, float* y, float* z) {
  float inclination;
  float azimuth;

  inclination = myRand() * PI;
  azimuth = myRand() * 2 * PI;

  *x = r * myRand() * sin(inclination) * cos(azimuth) + Cx; 
  *y = r * myRand() * sin(inclination) * sin(azimuth) + Cy;
  *z = r * myRand() * cos(inclination) + Cz;
}

void initParticles() {

  particles = (particle*) malloc(sizeof(particle) * PART_NO);

  int i;
  for(i = 0; i < PART_NO; i++) {

    randPointInSphere(0.0, 20000.0, 0.0, 8000, &particles[i].x, &particles[i].y, &particles[i].z);

//randPointOnSphere(0, 0, 0, 5000, &particles[i].x, &particles[i].y, &particles[i].z);

    particles[i].u = 0.0;
    particles[i].v = -300.0;
    particles[i].w = 550.0;

    particles[i].r = 1.0;
    particles[i].g = 0.0;
    particles[i].b = 0.0;

    particles[i].Au = 0.0;
    particles[i].Av = 0.0;
    particles[i].Aw = 0.0;

    particles[i].mass = (myRand() * PART_MASS_RANGE) + 10;

    particles[i].alive = true;
  }

  for(i = 0; i < MASS_PART_NO; i++) {
    mass_paritcles[i].x = 0.0;
    mass_paritcles[i].y = 0.0;
    mass_paritcles[i].z = 0.0;

    mass_paritcles[i].u = 0.0;
    mass_paritcles[i].v = 0.0;
    mass_paritcles[i].w = 0.0;

    mass_paritcles[i].r = 0.0;
    mass_paritcles[i].g = 0.0;
    mass_paritcles[i].b = 1.0;

    mass_paritcles[i].Au = 0.0;
    mass_paritcles[i].Av = 0.0;
    mass_paritcles[i].Aw = 0.0;

    mass_paritcles[i].mass = 5000000.0;

    particles[i].alive = false;
  }
}

void resetParticles() {

  free(particles);
  PART_NO = newPartNo;
  particles = (particle*) malloc(sizeof(particle) * PART_NO);
  int i;
  for(i = 0; i < PART_NO; i++) {

    randPointInSphere(0.0, 20000.0, 0.0, 8000, &particles[i].x, &particles[i].y, &particles[i].z);

//randPointOnSphere(0, 0, 0, 20000, &particles[i].x, &particles[i].y, &particles[i].z);

    particles[i].u = 0.0;
    particles[i].v = -300.0;
    particles[i].w = 550.0;

    particles[i].r = 1.0;
    particles[i].g = 0.0;
    particles[i].b = 0.0;

    particles[i].Au = 0.0;
    particles[i].Av = 0.0;
    particles[i].Aw = 0.0;

    particles[i].mass = (myRand() * PART_MASS_RANGE) + 1;

    particles[i].alive = true;
  }
}

///////////////////////////////////////////////

/*****************************/

void initStarCoords(void) {
  int i;
  float inclination;
  float azimuth;
  for(i = 0; i < STAR_NO; i++) {
    inclination = myRand() * 2 * PI;
    azimuth = myRand() * 2 * PI;

    star_coods[i][0] = star_radius * sin(inclination) * cos(azimuth); 
    star_coods[i][1] = star_radius * sin(inclination) * sin(azimuth);
    star_coods[i][2] = star_radius * cos(inclination);
  }
}

//taken from previous years lab ex2
void drawStarfield (void)
{
  glPointSize(1.0f);
  glBegin(GL_POINTS);
  glColor3f(1.0f, 1.0f, 1.0f);
  int i;

  for(i = 0; i < STAR_NO; i++) {
    glVertex3f(star_coods[i][0], star_coods[i][1], star_coods[i][2]);
  }

  glEnd();
}

void animate(void)
{
  if(!pause) {
    calcParticleValues();
    glutPostRedisplay();
    current_time++;
  }
}

double degToRad(int degrees) {
  return degrees / 360.0 * 2.0 * PI;
}

void drawTextInfo() {
  char intToString[20] = {};
  char message[300] = {};

  if(pause) {
    strcat(message, "PAUSED (p)\n\n");    
  }
  else {
    strcat(message, "UNPAUSED (p)\n\n");
  }

  sprintf(intToString, "%d", newPartNo);
  strcat(message, "Particle Number (n): ");
  strcat(message, intToString);
  strcat(message, "\n");

  sprintf(intToString, "%.2f", TIME);
  strcat(message, "Speed (t): ");
  strcat(message, intToString);
  strcat(message, "\n");

  sprintf(intToString, "%.0f", mass_paritcles[0].mass);
  strcat(message, "Mass (m): ");
  strcat(message, intToString);
  strcat(message, "\n");

  strcat(message, "Perspective (c): ");
  if(perspective)
    strcat(message, "ON\n");
  else
    strcat(message, "OFF\n");

  strcat(message, "\nSPACE to reset\n");
  strcat(message, "\np to pause, required to change particle number\n");
  strcat(message, "\nPress g to change particle type\n");
  strcat(message, "Use UPPERCASE to increase values\n");
  strcat(message, "w a s d to move \n");
  strcat(message, "z x to zoom\n");

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(0, 1000);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, message);
}

void calcCameraPos() {
  cameraX = cameraRadius * sin(degToRad(cameraLat)) * cos(degToRad(cameraLon)); 
  cameraY = cameraRadius * sin(degToRad(cameraLat)) * sin(degToRad(cameraLon));
  cameraZ = cameraRadius * cos(degToRad(cameraLat));
}

void display()
{
  glLoadIdentity();
  calcCameraPos();
  gluLookAt(cameraX, cameraY, cameraZ,
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0);
// Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawTextInfo();
  drawStarfield();
  drawParticles();
  glutSwapBuffers();
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
  if(key == 27) exit(0);
  if(key == 'a') cameraLon = (cameraLon + 5) % 360;
  if(key == 'd') cameraLon = (cameraLon - 5) % 360;
  if(key == 'w') {
    cameraLat = cameraLat + 5;
    if (cameraLat > 175)
      cameraLat = 175;
  }
  if(key == 's') {
    cameraLat = cameraLat - 5;
    if (cameraLat < 5) 
      cameraLat = 5;
  }
  if(key == 'p') pause = !pause;
  if(key == ' ') resetParticles();
  if(key == 'z') {
    cameraRadius += 500;
    if(cameraRadius > MAX_CAMERA)
      cameraRadius = MAX_CAMERA;
  }
  if(key == 'x') {
    cameraRadius -= 500;
    if(cameraRadius < (-1*MAX_CAMERA))
      cameraRadius = -1*MAX_CAMERA;
  }
  if(key == 'g') {
    renderMode = (renderMode + 1) % 3;

    if(renderMode == 1)
      glEnable(GL_POINT_SPRITE);
    else
      glEnable(GL_POINT_SPRITE);
  }
  if(key == 'm') {
    int j;
    for(j = 0; j < MASS_PART_NO; j++)
      if(mass_paritcles[j].mass > 50000.0)
        mass_paritcles[j].mass = mass_paritcles[j].mass - 50000.0;

  }
  if(key == 'M') {
  int j;
  for(j = 0; j < MASS_PART_NO; j++)
    mass_paritcles[j].mass = mass_paritcles[j].mass + 50000.0;

  }
  if(key == 't') {
    if(TIME > 0.05) TIME = TIME - 0.05;
  }
  if(key == 'T') {
    if(TIME < 1.0) TIME = TIME + 0.05;
  }
  if(key == 'c') perspective = !perspective;
  if(key == 'n' && pause && newPartNo > 2) {
    newPartNo /= 2;
    resetParticles();
  }
  if(key == 'N' && pause) {
    newPartNo *= 2;
    resetParticles();
  }
  glutPostRedisplay();
}

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000000000000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(1600, 1200);
  glutInitWindowPosition(100, 100);

  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("COMP37111 Particles");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);

  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_POINT_SPRITE);

  glAlphaFunc(GL_GREATER, 0);
}

//Code from (17-Oct-2016)
//http://stackoverflow.com/questions/16927442/loading-texture-using-soils-ogl-function-and-opengl
void loadTexture(GLuint* texture, char* filename){
  *texture = SOIL_load_OGL_texture(filename,
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA
    );
  if(texture == NULL){
    printf("[Texture loader] \"%s\" failed to load!\n", filename);
  }
}

int main(int argc, char *argv[])
{
  srand(time(NULL));
  initGraphics(argc, argv);
  initStarCoords();
  initParticles();

  loadTexture(&fuzzyTexture, "fire.png");

  glutIdleFunc (animate);

  glutMainLoop();

  return 1;
}