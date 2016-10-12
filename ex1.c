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
#include <stdbool.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#define PI 3.14159
#define STAR_NO 1000
#define PART_NO 10000
#define MASS_PART_NO 1
#define G 1.0
#define TIME 1.0
#define DECAY 0.1

// Display list for coordinate axis 
GLuint axisList;

GLint AXIS_SIZE= 500;
int axisEnabled= 1;
int star_radius = 200000;
int current_time = 0;

int left_right = 0;

GLfloat star_coods[STAR_NO][3];

typedef struct { 
  GLfloat r, g, b; //color
  GLfloat x, y, z; //position
  GLfloat u, v, w; //velocity vector
  GLfloat Au, Av, Aw; //accel vector
  GLfloat mass; //mass of the particle
  bool alive;
 } particle;

particle particles[PART_NO];
particle mass_paritcles[MASS_PART_NO];

float myRand (void)
{
  /* return a random float in the range [0,1] */

  return ((float) rand() / RAND_MAX);
}

void calcParticleValues() {
	int i;
	int j;
	float r;
	for(i = 0; i < PART_NO; i++) {
		if(particles[i].alive) {
			//calculate new position according to velocity
				particles[i].x += particles[i].u * TIME;
				particles[i].y += particles[i].v * TIME;
				particles[i].z += particles[i].w * TIME;
	
				for(j = 0; j < MASS_PART_NO; j++) {
	
					//check if particle is too close to mass(is destoryed if so)
					//approximated to a cube
					if(((particles[i].x < (mass_paritcles[j].x + mass_paritcles[j].mass / 10)) && (particles[i].x > (mass_paritcles[j].x - mass_paritcles[j].mass / 10)))
					&& ((particles[i].y < (mass_paritcles[j].y + mass_paritcles[j].mass / 10)) && (particles[i].y > (mass_paritcles[j].y - mass_paritcles[j].mass / 10)))
					&& ((particles[i].z < (mass_paritcles[j].z + mass_paritcles[j].mass / 10)) && (particles[i].z > (mass_paritcles[j].z - mass_paritcles[j].mass / 10)))) {
	
						particles[i].mass = false;
					}
				}
	
	
				r = sqrt(pow(particles[i].u, 2) + pow(particles[i].v, 2) + pow(particles[i].w, 2));
	
			//calculate new velocity according to acceleration (force)
			//some velocity is taken away to simulate orbit decay
			particles[i].u += particles[i].Au * TIME - DECAY * (particles[i].u / r);
			particles[i].v += particles[i].Av * TIME - DECAY * (particles[i].v / r);
			particles[i].w += particles[i].Aw * TIME - DECAY * (particles[i].w / r);
		}
	}
}

void calcParticleAccel() {
	int i;
	int j;
	float force;
	float r;
	float diffX;
	float diffY;
	float diffZ;
	for(i = 0; i < PART_NO; i++) {
		if(particles[i].alive){
			for(j = 0; j < MASS_PART_NO; j++) {
					//calculate distance between the objects
					diffX = mass_paritcles[j].x - particles[i].x;
					diffY = mass_paritcles[j].y - particles[i].y;
					diffZ = mass_paritcles[j].z - particles[i].z;
	
					r = sqrt(pow(diffX, 2) + pow(diffY, 2) + pow(diffZ, 2));
	
					force = (G * mass_paritcles[j].mass * particles[i].mass) / r;
	
				if(diffX != 0)
					particles[i].Au = (diffX / r * force) / particles[i].mass;
				else
					particles[i].Au = 0.0;
				if(diffY != 0)
					particles[i].Av = (diffY / r * force) / particles[i].mass;
				else
					particles[i].Av = 0.0;
				if(diffZ != 0)
					particles[i].Aw = (diffZ / r * force) / particles[i].mass;
				else
					particles[i].Aw = 0.0;
			}
		}
	}
}

void initParticles() {
		int i;
	for(i = 0; i < PART_NO; i++) {
		particles[i].x = (myRand() * 5000 + 3000) * -1;
		particles[i].y = (myRand() * 5000 + 1000);
		particles[i].z = (myRand() * 5000 + 1000);

		particles[i].u = 60.0;
		particles[i].v = 2.0;
		particles[i].w = 4.0;

		particles[i].r = 0.0;
		particles[i].g = 1.0;
		particles[i].b = 0.0;

		particles[i].Au = 0.0;
		particles[i].Av = 0.0;
		particles[i].Aw = 0.0;

		particles[i].mass = myRand() * 100;

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

		mass_paritcles[i].mass = 2000.0;

		particles[i].alive = false;
	}
}

void drawParticles() {

	//draw particles
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	int i;

	for(i = 0; i < PART_NO; i++) {
		if(particles[i].alive) {
			glColor3f(particles[i].r, particles[i].g, particles[i].b);
			glVertex3f(particles[i].x, particles[i].y, particles[i].z);
		}
	}
	glEnd();

	//draw massive particles
	glPointSize(10.0f);
	glBegin(GL_POINTS);

	for(i = 0; i < MASS_PART_NO; i++) {
		glColor3f(mass_paritcles[i].r, mass_paritcles[i].g, mass_paritcles[i].b);
		glVertex3f(mass_paritcles[i].x, mass_paritcles[i].y, mass_paritcles[i].z);
	}
	glEnd();
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

//taken from previous years lab
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
		calcParticleValues();
		calcParticleAccel();
    glutPostRedisplay();
    current_time++;
}

///////////////////////////////////////////////

void display()
{
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 30000.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);
  // If enabled, draw coordinate axis

  glPushMatrix();
  if(axisEnabled) glCallList(axisList);
  drawStarfield();
  drawParticles();
	glRotatef(left_right,0.0,1.0,0.0);
	glPopMatrix();

  glutSwapBuffers();
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
  if(key == 27) exit(0);
  glutPostRedisplay();
}

void cursor_keys(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_LEFT:
  	left_right = (left_right - 1) % 360;
    break;
  case GLUT_KEY_RIGHT:
  	left_right = (left_right + 1) % 360;
    break;
  }
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

void makeAxes() {
// Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}

///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(1600, 1200);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("COMP37111 Particles");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  //double f;
  srand(time(NULL));
  initGraphics(argc, argv);

  initStarCoords();
  initParticles();
  glutSpecialFunc(cursor_keys);

  glEnable(GL_POINT_SMOOTH);
  glutIdleFunc (animate);
  glutMainLoop();

  return 1;
}
