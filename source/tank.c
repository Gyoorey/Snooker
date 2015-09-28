// ShinyJet.c
// OpenGL SuperBible, Chapter 6
// Demonstrates OpenGL Lighting
// Program by Richard S. Wright Jr.

/*
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
*/
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
/*
#include <math.h>
*/


// Define a constant for the value of PI
#define GL_PI 3.1415f

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
	{
	float length;
	
	// Calculate the length of the vector		
	length = (float)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
	{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
	}


// Called to draw scene
void RenderScene(void)
	{
	float normal[3];	// Storeage for calculated surface normal
	char str[20];
	int x,y;
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	
		do{
			scanf("%s %d %d\n",str,&x,&y);
			printf("Beolvasott: %s %d %d\n",str,x,y);
			if( !strcmp(str,"end") ){
				if( strcmp(str,"red") ){
					glPushMatrix();
						printf("Gomb: %d %d\n",x,y);
						glColor3f(1.0, 0.0, 0.0);
						glTranslatef( (float)(x-89), (float)(y-178), 2.5);
						glutSolidSphere(5,20,20);
					glPopMatrix();
				}
			}
		}while( !strcmp(str,"end") );

		glPushMatrix();
			glColor3f(1.0, 0.0, 0.0);
			glTranslatef( (float)(x-89), (float)(y-178), 2.5);
			glutSolidSphere(5,20,20);
		glPopMatrix();


		glColor3f(0.0, 1.0, 0.0);
		glPushMatrix();
			glScalef(1.0, 2.0, 0.1);
			glutSolidCube(178);
		glPopMatrix();
		
	//glTranslatef(0.0, 21.0, 0.0);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);


	// Restore the matrix state
	glPopMatrix();
	// Display the results
	glutSwapBuffers();
	}

// This function does any needed initialization on the rendering
// context. 
void SetupRC()
	{
	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat  specref[] =  { 0.2f, 0.2f, 0.2f, 1.0f };

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	//glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	//glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,20);

	// Light black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	}

void SpecialKeys(int key, int x, int y)
	{
	if(key == GLUT_KEY_UP)
		xRot-= 5.0f;

	if(key == GLUT_KEY_DOWN)
		xRot += 5.0f;

	if(key == GLUT_KEY_LEFT)
		yRot -= 5.0f;

	if(key == GLUT_KEY_RIGHT)
		yRot += 5.0f;

	if(xRot > 10.0f)
		xRot = 10.0f;

	if(xRot < -50.0f)
		xRot = -50.0f;

	if(yRot > 356.0f)
		yRot = 0.0f;

	if(yRot < -1.0f)
		yRot = 355.0f;

	// Refresh the Window
	glutPostRedisplay();
	}


void ChangeSize(int w, int h)
	{
	GLfloat nRange = 250.0f;
	GLfloat	 lightPos[] = { -50.f, 50.0f, 100.0f, 1.0f };

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
    if (w <= h) 
		glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
    else 
		glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	}

int main(int argc, char* argv[])
	{
GLubyte *extensions;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Shiny Jet");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
extensions = glGetString(GL_EXTENSIONS);

if (strstr(extensions, "GL_ARB_MULTI_TEXTURE") != 0)
	printf("van multi texture\n");
printf("%s\n", extensions);
	glutMainLoop();

	return 0;
	}
