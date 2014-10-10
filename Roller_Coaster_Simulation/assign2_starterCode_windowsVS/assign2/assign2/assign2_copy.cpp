// assign2.cpp : Defines the entry point for the console application.
//

/*
CSCI 480 Computer Graphics
Assignment 2: Simulating a Roller Coaster
C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glu.h>
#include <GL/glut.h>

# define PI 3.141592
//# define SKY
#pragma warning (disable : 4996)

/* represents one control point along the spline */
typedef struct point {
	double x;
	double y;
	double z;
}_point;

/* spline struct which contains how many control points, and an array of control points */
typedef struct spline {
	int numControlPoints;
	struct point *points;
}_spline;

/*
* Structures for Tangent, Normal and Bi-normal
*/
_point tangentVector, normalVector, binormalVector;

/* the spline array */
_spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;
/* Index variable to track the number of splines */
int index;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

int renderValue = 0;
int rotateValue = 4;
int reset = 0;
int zAxis = 1;
int startAnimation = 0;
int startCapture = 0;
int frame = 0;
int startLighting = 0;

GLfloat screenWidth = 0;
GLfloat screenHeight = 0;
GLfloat xCenter = 0.0;
GLfloat yCenter = 0.0;
GLfloat hStep = 0.01;
GLfloat vStep = -0.01;


int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

GLfloat theta[3] = {0.0, 0.0, 0.0}; 
GLfloat delta = 2.00; 
GLint axis = 2; 

/* state of the world */
float g_vLandRotate[3] = {0.0, 1.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

Pic * g_pHeightData;

static GLubyte checkImage[1920][1440][4];

GLuint sky;
/* Write a screenshot to the specified filename */
void saveScreenshot ( char *filename )
{
	int i, j;
	Pic *in = NULL;

	if ( filename == NULL ) {
		return;
	}

	/* Allocate a picture buffer */
	in = pic_alloc( 640, 480, 3, NULL );

	printf( "File to save to: %s\n", filename );

	for (i = 479; i >= 0; i--) {
		glReadPixels( 0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp] );
	}

	if ( jpeg_write( filename, in ) ) {
		printf( "File saved Successfully\n ");
	} else {
		printf( "Error in Saving\n" );
	}

	pic_free( in );
}

int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf ("can't open file\n");
		exit(1);
	}

	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));

	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf ("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
		g_Splines[j].numControlPoints = iLength;

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf", 
			&g_Splines[j].points[i].x, 
			&g_Splines[j].points[i].y, 
			&g_Splines[j].points[i].z) != EOF) {
				i++;
		}
	}
	printf ( "Number of Splines : %d\n", g_iNumOfSplines );

	free(cName);

	return 0;
}

void myReshape( int w, int h ) 
{ 
	GLfloat aspect = (GLfloat) w / (GLfloat) h; 
	glViewport(0, 0, w, h); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	if (w <= h) /* aspect <= 1 */  {
		glFrustum ( -10.0, 10.0, -10.0/aspect, 10.0/aspect, -10.0, 10.0 );
	} else /* aspect > 1 */ { 
		glFrustum ( -10.0/aspect, 10.0/aspect, -10.0, 10.0, -10.0, 10.0 );
	}
	glMatrixMode(GL_MODELVIEW); 
}

void doIdle()
{
	/* make the screen update */
	glutPostRedisplay();
}

void myinit ()
{
	/* setup gl view here */
	/*
	* Set the background color as black and the coloring mode as smooth
	*/
	glClearColor ( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel ( GL_FLAT );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &sky);
	glBindTexture(GL_TEXTURE_2D, sky);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_pHeightData->nx, g_pHeightData -> ny, 0, GL_RGB, GL_UNSIGNED_BYTE, g_pHeightData->pix);
}

/* converts mouse drags into information about 
* rotation/translation/scaling 
*/
void mousedrag( int x, int y )
{
	int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};

	switch (g_ControlState)
	{
	case TRANSLATE:  
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0]*0.01;
			g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1]*0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[1] += vMouseDelta[0];
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
			g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
	glutPostRedisplay();
}

/* 
* Returnt the current mouse pointer location when there is no activity of the mouse
*/
void mouseidle( int x, int y )
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

/*
* Controls the state in which the mouse operation will occur
* If the mouse is left clicked then the image starts rotating, 
* if the mouse is left clicked with pressing down the control 
* button then the object gets translated according to the motion 
* of the mouse. If shift key is pressed along with the mouse then
* the object gets scaled.
*/
void mousebutton( int button, int state, int x, int y ) {
	switch ( button )
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state==GLUT_DOWN);
		break;
	}

	switch(glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

GLuint stop = 1;
/*
* Actions for the keyboard press are defined in this function
*/
void keyboard(unsigned char key, int x, int y) 
{ 
	// If 'q' or 'Q' is pressed then quit the program
	if ( key == 'q' || key == 'Q' ) {
		exit(0); 
	} else if ( key == ' ' )  { // If space bar is pressed then stop rotating
		stop = !stop; 
	} else if ( key == 'w' || key == 'W' ) { // Pressing 'w' key will render the image as points
		// The wire frame mode is disabled before making the object render as points
		glPolygonMode( GL_FRONT, GL_FILL );
		glPolygonMode( GL_BACK, GL_FILL );
		renderValue = 0;
		glutPostRedisplay();
	} else if ( key == 'e' || key == 'E' ) { // Pressing 'e' key will render the image as lines
		// The wire frame mode is enabled and the z-fighting is circumvented by using the glEnable function
		glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glEnable ( GL_POLYGON_OFFSET_FILL );
		renderValue = 1;
		glutPostRedisplay();
	} else if ( key == 'r' || key == 'R' ) { // Pressing the 'r' key will render the image as triangles
		glPolygonMode ( GL_FRONT_AND_BACK, GL_TRIANGLES );
		glEnable ( GL_POLYGON_OFFSET_FILL );
		renderValue = 2;
		glutPostRedisplay();
	} else if ( key == 't' || key == 'T' ) { // Pressing the 't' key will render the image as triangle strip
		glPolygonMode( GL_FRONT, GL_FILL );
		glPolygonMode( GL_BACK, GL_FILL );
		renderValue = 3;
		glutPostRedisplay();
	}

	// Pressing the 'a' key will enable the roll rotation mode of the object
	if ( key == 'a' || key == 'A' ) {
		rotateValue = 0;
	} else if ( key == 's' || key == 'S' ) { // Pressing the 's' key will enable the pitch rotation mode of the object
		rotateValue = 1;
	} else if ( key == 'd' || key == 'D' ) { // Pressing the 'd' key will enable the yaw rotation mode of the object
		rotateValue = 2;
	} else if ( key == 'f' || key == 'F' ) { // Pressing the 'f' key will stop all kinds of rotation mode
		rotateValue = 3;
	} 

	// Pressing the 'm' key will start the animation
	if ( key == 'm' || key == 'M' ) {
		startAnimation = ( startAnimation == 1 ) ? 0 : 1;
	}

	// Pressing the 'c' key will start the capturing of the images.
	if ( key == 'c' || key == 'C' ) {
		startCapture = ( startCapture == 1 ) ? 0 : 1;
	}

	// Pressing key 'l' will toggle the lighting
	if ( key == 'l' || key == 'L' ) {
		startLighting = ( startLighting == 1 ) ? 0 : 1;
	}

	//glutIdleFunc(spinCube); 
	glutPostRedisplay();
	if (stop) {
		glutIdleFunc(NULL); 
	} else {
		//glutIdleFunc(spinCube); 
	}
	//glutIdleFunc(spinCube); 
	glutPostRedisplay();
} 

/*
* sceneLighting function defines the different types of light sources and their relative position
* in the scene.
*/
void sceneLighting ()
{
	if ( startLighting == 1 )
	{
		// Enable Lighting in OpenGL
		glEnable ( GL_LIGHTING );
		// Enable Light source 0
		glEnable ( GL_LIGHT0 );
		// Enable Light Source 1
		glEnable ( GL_LIGHT1 );
		glEnable ( GL_NORMALIZE );
		/*
		* Ambient Light source is the light source which lights up each and every surface the same way.
		* The ambient light source is modelled using the glLightModelfv command.
		*/
		GLfloat ambientLightSource[] = { 0.8, 0.2, 0.3, 1.0 };
		glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, ambientLightSource );
		// Adding a Positioned light source into the scene
		// Green dominant color source
		GLfloat posLight[]={0.2f,0.7f,0.2f,1.0f};
		// Position the light source at (5, 5, 0)
		GLfloat posLightPosition[] = { 5.0f, 5.0f, 0.0f, 1.0f };
		glLightfv ( GL_LIGHT0, GL_DIFFUSE, posLight );
		glLightfv ( GL_LIGHT0, GL_POSITION, posLightPosition );
		glShadeModel (GL_SMOOTH );

		// Add a directed light
		GLfloat directedLight[] = { 0.2f, 0.2f, 0.9f, 1.0f };// Blue dominated light
		// Setting the last value in the position to 0.0 differentiates the light source
		// as a direted light source instead of a positioned light source
		GLfloat directedLightPosition[]={ -5.0f, -5.0f, 0.0f, 0.0f };
		glLightfv ( GL_LIGHT1, GL_DIFFUSE, directedLight );
		glLightfv ( GL_LIGHT1, GL_POSITION, directedLightPosition );
	} else {
		glDisable ( GL_LIGHTING );
	}
}

/*
 * Obtain the X co-ordinate value in the cartesian co-ordinate system when provided with the radius, angle wrt Z axis 
 * and X axi.s
 */
float getXCoordinate ( float angle, float radiusAngle, float domeRadius ) {
	return sin(radiusAngle) * cos(angle) * domeRadius;
}

/*
 * Obtain the Z co-ordinate value in the cartesian co-ordinate system when provided with the radius, angle wrt Z axis 
 * and X axi.s
 */
float getZCoordinate ( float angle, float radiusAngle, float domeRadius ) {
	return sin(radiusAngle) * sin(angle) * domeRadius;
}

/*
 * Obtain the Y co-ordinate value in the cartesian co-ordinate system when provided with the radius, angle wrt Z axis 
 * and X axi.s
 */
float getYCoordinate ( float domeRadius, float pointRadiusAngle ) {
	return (cos(pointRadiusAngle) * domeRadius) / 2;
}

void drawSkyDome (float radius, int longitudes, int latitudes) {
	const float centerRad = (( 2 * PI )/360.0) * (1);
	int i, j;
	float XCoordinate, YCoordinate, ZCoordinate;
	float imageX, imageY;
	float incrLongitudes = ( 2 * PI ) / longitudes;
	float incrLatitudes = (( 3 * PI / 2 ) - centerRad) / (latitudes - 1);
	//glNormal3f(0, -1, 0);
	glEnable ( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, sky );
	for ( i = 0; i < longitudes; i = i + 1 ) {
		glBegin(GL_QUAD_STRIP);
		for ( j = 0; j < latitudes; j = j + 1 ) {
			XCoordinate = getXCoordinate ( ( i + 1 ) * incrLongitudes, ( j * incrLatitudes ) + centerRad, radius );
			YCoordinate = getYCoordinate ( radius, ( j * incrLatitudes) + centerRad );
			ZCoordinate = getZCoordinate ( ( i + 1) * incrLongitudes, ( j * incrLatitudes ) + centerRad, radius );
			imageX = ( i + 1 ) * incrLongitudes / ( 2 * PI );
			imageY = ( j * incrLatitudes ) / ( PI / 2 );
			glTexCoord2f( imageX, imageY );
			//glTexCoord3f( XCoordinate, YCoordinate, ZCoordinate );
			glVertex3f( XCoordinate, YCoordinate, ZCoordinate );

			XCoordinate = getXCoordinate ( i * incrLongitudes, ( j * incrLatitudes ) + centerRad, radius );
			ZCoordinate = getZCoordinate ( i * incrLongitudes, ( j * incrLatitudes ) + centerRad, radius );
			imageX = ( i   * incrLongitudes ) / ( 2 * PI );
			imageY = ( j * incrLatitudes ) / ( PI / 2 );
			glTexCoord2f( imageX, imageY );
			//glTexCoord3f ( XCoordinate, YCoordinate, ZCoordinate );
			glVertex3f ( XCoordinate, YCoordinate, ZCoordinate );
		}
		glEnd();
	}

# ifdef WIREFRAME
	for ( i = 0; i < longitudes; i = i + 1 ) {
		glBegin ( GL_LINES );
		for (int j = 0; j < latitudes; ++j) {

			/*
			* ( i + 1, j ) and ( i, j )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			// YCoordinate doesn't change
			ZCoordinate = getZCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j + 1 ) and ( i, j )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j ) and ( i + 1, j )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j ) and ( i, j + 1 )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate(i * incrLongitudes, ((j+1) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////

			/*
			* ( i, j + 1 ) and ( i, j )
			*/
			XCoordinate = getXCoordinate((i) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate((i) * incrLongitudes, ((j + 1) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, ((j) * incrLatitudes) + centerRad, radius);
			YCoordinate = getYCoordinate(radius, ((j) * incrLatitudes) + centerRad);
			ZCoordinate = getZCoordinate(i * incrLongitudes, ((j) * incrLatitudes) + centerRad, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////
		}
		glEnd();
	}
# endif
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0);
	glTexCoord2f(1, 0);
	glVertex3f(0, getYCoordinate(radius, 0), 0);
	for (i = 0; i <= longitudes; ++i) {
		XCoordinate = getXCoordinate(i * incrLongitudes, centerRad, radius);
		YCoordinate = getYCoordinate(radius, centerRad);
		ZCoordinate = getZCoordinate(i * incrLongitudes, centerRad, radius);
		//glTexCoord3f( XCoordinate, YCoordinate, ZCoordinate );
		glVertex3f(XCoordinate, YCoordinate, ZCoordinate);
	}
	glEnd();
	glDisable ( GL_TEXTURE_2D );
	glDepthMask( GL_TRUE );
}

void drawCube () {

	glBegin(GL_POLYGON);
	glEnable ( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, sky );
	glDepthMask( GL_TRUE );

	// Z = 0.5
	glTexCoord2f (2048.0/2048.0, 1023.0/1536.0); glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f (2048.0/2048.0, 512.0/1536.0); glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f (1536.0/2048.0, 512.0/1536.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f (1536.0/2048.0, 1023.0/1536.0); glVertex3f(0.5, -0.5, 0.5);
	/*
	// Z = -0.5
	glTexCoord2f (512.0/2048.0, 1023.0/1536.0); glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f (512.0/2048.0, 512.0/1536.0); glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f (1023.0/2048.0, 512.0/1536.0); glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f (1023.0/2048.0, 1023.0/1536.0); glVertex3f(-0.5, -0.5, -0.5);
	*/
	// X = -0.5
	glTexCoord2f (511.0/2048.0, 1023.0/1536.0); glVertex3f(-0.5, -0.5, -0.5);
	glTexCoord2f (511.0/2048.0, 512.0/1536.0); glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f (0.0/2048.0, 512.0/1536.0); glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f (0.0/2048.0, 1023.0/1536.0); glVertex3f(-0.5, 0.5, -0.5);
	/*
	// X = 1.0
	glTexCoord2f (1535.0/2048.0, 1023.0/1536.0); glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f (1535.0/2048.0, 512.0/1536.0); glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f (1024.0/2048.0, 512.0/1536.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f (1024.0/2048.0, 1023.0/1536.0); glVertex3f(0.5, 0.5, -0.5);

	// Y = 1.0
	glTexCoord2f (1023.0/2048.0, 511.0/1536.0); glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f (1023.0/2048.0, 0.0/1536.0); glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f (512.0/2048.0, 0.0/1536.0); glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f (512.0/2048.0, 511.0/1536.0); glVertex3f(0.5, 0.5, -0.5);
	/*
	// Y = 0
	glTexCoord2f (1023.0/2048.0, 1536.0/1536.0); glVertex3f(-0.5, -0.5, -0.5);
	glTexCoord2f (1023.0/2048.0, 1024.0/1536.0); glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f (512.0/2048.0, 1024.0/1536.0); glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f (512.0/2048.0, 1536.0/1536.0); glVertex3f(0.5, -0.5, -0.5);
	*/
	glEnd();
}

/*
 * Compute the 3D world point represented by the parameter u
 */
_point find3DPoint ( GLfloat u ) {
	_point Point;
	int i, j, k;
	GLfloat s = 0.5;

	GLfloat Matrix1[1][4] = { { u * u * u, u * u, u, 1 } };
	GLfloat Basis[4][4] = { { -s, 2 - s, s - 2, s }, { 2 * s, s - 3, 3 - ( 2 * s ), -s }, { -s, 0, s, 0 }, { 0, 1, 0, 0 } };
	GLfloat interMatrix[4][3];
	GLfloat controlMatrix[4][3];

	Point.x = 0.0;
	Point.y = 0.0;
	Point.z = 0.0;

	if ( index == 0 ) {

		controlMatrix[ 0 ][ 0 ] =  0.0;
		controlMatrix[ 0 ][ 1 ] =  0.0;
		controlMatrix[ 0 ][ 2 ] =  0.0;

		for ( i = 1; i < 4; i = i + 1 ) {
			controlMatrix[ i ][ 0 ] =  g_Splines[ 0 ].points[ index + i ].x;
			controlMatrix[ i ][ 1 ] =  g_Splines[ 0 ].points[ index + i ].y;
			controlMatrix[ i ][ 2 ] =  g_Splines[ 0 ].points[ index + i ].z;
		}
	}
	else {

		for ( i = 0; i < 4; i = i + 1 ) {
			controlMatrix[ i ][ 0 ] =  g_Splines[ 0 ].points[ index - 1 + i ].x;
			controlMatrix[ i ][ 1 ] =  g_Splines[ 0 ].points[ index - 1 + i ].y;
			controlMatrix[ i ][ 2 ] =  g_Splines[ 0 ].points[ index - 1 + i ].z;
		}
	}

	/*
	* Initialize the intermediate matrix which holds the matrix multipication value of the control and the basis matrix to 0
	*/
	for ( i = 0; i < 4; i = i + 1 ) {
		for ( j = 0; j < 3; j = j + 1 ) {
			interMatrix[i][j] = 0.0;
		}
	}

	/* 
	* Multiply the basis and control matrices
	*/

	for ( i = 0; i < 4; i = i + 1 ) {
		for ( j = 0; j < 3; j = j + 1 ) {
			for ( k = 0; k < 4; k = k + 1 ) {
				interMatrix[ i ][ j ] = interMatrix[ i ][ j ] + ( Basis[ i ][ k ] * controlMatrix[ k ][ j ] ); 
			}
		}
	}

	/*
	* Multiply the intermediate matrix with the paramter matrix to get the points
	*/
	for ( j = 0; j < 4; j = j + 1 ) {
		Point.x = Point.x + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 0 ] );
		Point.y = Point.y + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 1 ] );
		Point.z = Point.z + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 2 ] );
	}

	return Point;
}

/*
 * Compute the tangent vector at the point defined by the parameter u
 */
_point findTangent ( GLfloat u ) {
	_point Point;
	int i, j, k;
	GLfloat s = 0.5;

	GLfloat Matrix1[1][4] = { { 3 * u * u, 2 * u, 1, 0 } };
	GLfloat Basis[4][4] = { { -s, 2 - s, s - 2, s }, { 2 * s, s - 3, 3 - ( 2 * s ), -s }, { -s, 0, s, 0 }, { 0, 1, 0, 0 } };
	GLfloat interMatrix[4][3];
	GLfloat controlMatrix[4][3];
	GLfloat magPoint = 0.0;

	Point.x = 0.0;
	Point.y = 0.0;
	Point.z = 0.0;

	if ( index == 0 ) {

		controlMatrix[ 0 ][ 0 ] =  0.0;
		controlMatrix[ 0 ][ 1 ] =  0.0;
		controlMatrix[ 0 ][ 2 ] =  0.0;

		for ( i = 1; i < 4; i = i + 1 ) {
			controlMatrix[ i ][ 0 ] =  g_Splines[ 0 ].points[ index + i ].x;
			controlMatrix[ i ][ 1 ] =  g_Splines[ 0 ].points[ index + i ].y;
			controlMatrix[ i ][ 2 ] =  g_Splines[ 0 ].points[ index + i ].z;
		}
	}
	else {

		for ( i = 0; i < 4; i = i + 1 ) {
			controlMatrix[ i ][ 0 ] =  g_Splines[ 0 ].points[ index - 1 + i ].x;
			controlMatrix[ i ][ 1 ] =  g_Splines[ 0 ].points[ index - 1 + i ].y;
			controlMatrix[ i ][ 2 ] =  g_Splines[ 0 ].points[ index - 1 + i ].z;
		}
	}

	/*
	* Initialize the intermediate matrix which holds the matrix multipication value of the control and the basis matrix to 0
	*/
	for ( i = 0; i < 4; i = i + 1 ) {
		for ( j = 0; j < 3; j = j + 1 ) {
			interMatrix[i][j] = 0.0;
		}
	}

	/* 
	* Multiply the basis and control matrices
	*/

	for ( i = 0; i < 4; i = i + 1 ) {
		for ( j = 0; j < 3; j = j + 1 ) {
			for ( k = 0; k < 4; k = k + 1 ) {
				interMatrix[ i ][ j ] = interMatrix[ i ][ j ] + ( Basis[ i ][ k ] * controlMatrix[ k ][ j ] ); 
			}
		}
	}

	/*
	* Multiply the intermediate matrix with the paramter matrix to get the points
	*/
	for ( j = 0; j < 4; j = j + 1 ) {
		Point.x = Point.x + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 0 ] );
		Point.y = Point.y + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 1 ] );
		Point.z = Point.z + ( Matrix1[ 0 ][ j ] * interMatrix[ j ][ 2 ] );
	}

	magPoint = sqrt ( Point.x * Point.x + Point.y * Point.y + Point.z * Point.z );

	Point.x = Point.x / magPoint;
	Point.y = Point.y / magPoint;
	Point.z = Point.z / magPoint;

	return Point;
}

/*
 * Computes the cross product between two vectors vector1 and vector2
 */
_point vectorCrossProduct ( _point vector1, _point vector2 ) {
	_point crossProduct;

	crossProduct.x = ( vector1.y * vector2.z ) - ( vector1.z * vector2.y );
	crossProduct.y = ( vector1.z * vector2.x ) - ( vector1.x * vector2.z );
	crossProduct.z = ( vector1.x * vector2.y ) - ( vector1.y * vector2.x );

	return crossProduct;
}

void drawSpline ( GLfloat u0, GLfloat u1, GLfloat maxLineLength ) {

	_point Point1, Point2, tangentPoint1, tangentPoint2;
	_point normalPoint1, normalPoint2, binormalPoint1, binormalPoint2;
	GLfloat midu, distance, magNormal, magBinormal;
	_point trackDisplacement1, trackDisplacement2;
	_point arbVector, trackQuad11, trackQuad12, trackQuad13, trackQuad14;
	_point trackQuad21, trackQuad22, trackQuad23, trackQuad24;

	arbVector.x = 1;
	arbVector.y = 0;
	arbVector.z = 1;

	midu = ( u0 + u1 ) / 2;	

	Point1 = find3DPoint ( u0 );
	Point2 = find3DPoint ( u1 );

	distance = sqrt ( ( Point1.x - Point2.x ) * ( Point1.x - Point2.x ) + ( Point1.y - Point2.y ) * ( Point1.y - Point2.y ) + ( Point1.z - Point2.z ) * ( Point1.z - Point2.z ) );

	/*
	 * Calculate the distance between the two 3D world points. If the distance comes out to be more than the 
	 * maxLineLength value then recursively subdivide the problem. If the distance is less than the maxLineLength 
	 * then a line segment is drawn from Point1 to Point2
	 */
	if ( distance > maxLineLength ) {
		drawSpline ( u0, midu, maxLineLength );
		drawSpline ( midu, u1, maxLineLength );
	} else {

		/*
		 * Calculate the tangent vector using the formula t(u) = [3u^2 2u 1 0] M * C
		 */
		tangentPoint1 = findTangent ( u0 );
		tangentPoint2 = findTangent ( u1 );

		/*
		 * Calculate the normal vectors, which is the cross product of any arbitrary vector and the 
		 * tangent vector.
 		 */
		normalPoint1 = vectorCrossProduct ( tangentPoint1, arbVector );
		normalPoint2 = vectorCrossProduct ( tangentPoint2, arbVector );

		/*
		* Normalize the normal vectors
		*/

		magNormal = sqrt ( ( normalPoint1.x * normalPoint1.x ) + ( normalPoint1.y * normalPoint1.y ) + ( normalPoint1.z * normalPoint1.z ) );
		normalPoint1.x = normalPoint1.x / magNormal;
		normalPoint1.y = normalPoint1.y / magNormal;
		normalPoint1.z = normalPoint1.z / magNormal;

		magNormal = sqrt ( ( normalPoint2.x * normalPoint2.x ) + ( normalPoint2.y * normalPoint2.y ) + ( normalPoint2.z * normalPoint2.z ) );
		normalPoint2.x = normalPoint2.x / magNormal;
		normalPoint2.y = normalPoint2.y / magNormal;
		normalPoint2.z = normalPoint2.z / magNormal;

		/*
		 * Calculate the bi-normal vectors, which is the cross product of the tangent vector
		 * and the normal vector
		 */
		binormalPoint1 = vectorCrossProduct ( tangentPoint1, normalPoint1 );
		binormalPoint2 = vectorCrossProduct ( tangentPoint2, normalPoint2 );

		/*
		* Normalize the bi-normal vectors
		*/
		magBinormal = sqrt ( ( binormalPoint1.x * binormalPoint1.x ) + ( binormalPoint1.y * binormalPoint1.y ) + ( binormalPoint1.z * binormalPoint1.z ) );
		binormalPoint1.x = binormalPoint1.x / magBinormal;
		binormalPoint1.y = binormalPoint1.y / magBinormal;
		binormalPoint1.z = binormalPoint1.z / magBinormal;

		magBinormal = sqrt ( ( binormalPoint2.x * binormalPoint2.x ) + ( binormalPoint2.y * binormalPoint2.y ) + ( binormalPoint2.z * binormalPoint2.z ) );
		binormalPoint2.x = binormalPoint2.x / magBinormal;
		binormalPoint2.y = binormalPoint2.y / magBinormal;
		binormalPoint2.z = binormalPoint2.z / magBinormal;

		trackDisplacement1.x = 0.25 * binormalPoint1.x;
		trackDisplacement1.y = 0.25 * binormalPoint1.y;
		trackDisplacement1.z = 0.25 * binormalPoint1.z;

		trackDisplacement2.x = 0.25 * binormalPoint2.x;
		trackDisplacement2.y = 0.25 * binormalPoint2.y;
		trackDisplacement2.z = 0.25 * binormalPoint2.z;
		/*
		 * Compute the four points around the point to draw a cuboid around the spline
		 */
		trackQuad11.x = Point1.x + 0.001 * ( normalPoint1.x + binormalPoint1.x );  trackQuad11.y = Point1.y + 0.001 * ( normalPoint1.y + binormalPoint1.y );  trackQuad11.z = Point1.z + 0.001 * ( normalPoint1.z + binormalPoint1.z );
		trackQuad12.x = Point1.x + 0.001 * ( -normalPoint1.x + binormalPoint1.x ); trackQuad12.y = Point1.y + 0.001 * ( -normalPoint1.y + binormalPoint1.y ); trackQuad12.z = Point1.z + 0.001 * ( -normalPoint1.z + binormalPoint1.z );
		trackQuad13.x = Point1.x + 0.001 * ( -normalPoint1.x - binormalPoint1.x ); trackQuad13.y = Point1.y + 0.001 * ( -normalPoint1.y - binormalPoint1.y ); trackQuad13.z = Point1.z + 0.001 * ( -normalPoint1.z - binormalPoint1.z );
		trackQuad14.x = Point1.x + 0.001 * ( normalPoint1.x - binormalPoint1.x );  trackQuad14.y = Point1.y + 0.001 * ( normalPoint1.y - binormalPoint1.y );  trackQuad14.z = Point1.z + 0.001 * ( normalPoint1.z - binormalPoint1.z );

		trackQuad21.x = Point2.x + 0.001 * ( normalPoint2.x + binormalPoint2.x );  trackQuad21.y = Point2.y + 0.001 * ( normalPoint2.y + binormalPoint2.y );  trackQuad21.z = Point2.z + 0.001 * ( normalPoint2.z + binormalPoint2.z );
		trackQuad22.x = Point2.x + 0.001 * ( -normalPoint2.x + binormalPoint2.x ); trackQuad22.y = Point2.y + 0.001 * ( -normalPoint2.y + binormalPoint2.y ); trackQuad22.z = Point2.z + 0.001 * ( -normalPoint2.z + binormalPoint2.z );
		trackQuad23.x = Point2.x + 0.001 * ( -normalPoint2.x - binormalPoint2.x ); trackQuad23.y = Point2.y + 0.001 * ( -normalPoint2.y - binormalPoint2.y ); trackQuad23.z = Point2.z + 0.001 * ( -normalPoint2.z - binormalPoint2.z );
		trackQuad24.x = Point2.x + 0.001 * ( normalPoint2.x - binormalPoint2.x );  trackQuad24.y = Point2.y + 0.001 * ( normalPoint2.y - binormalPoint2.y );  trackQuad24.z = Point2.z + 0.001 * ( normalPoint2.z - binormalPoint2.z );
		/*
		 * Render the first track
		 */
		glEnable ( GL_DEPTH );
		glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x, Point1.y, Point1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x, Point2.y, Point2.z );
		glEnd ();
		/*glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad11.x, trackQuad11.y, trackQuad11.z ); 
		glVertex3f ( trackQuad21.x, trackQuad21.y, trackQuad21.z ); 
		glVertex3f ( trackQuad14.x, trackQuad14.y, trackQuad14.z ); 
		glVertex3f ( trackQuad24.x, trackQuad24.y, trackQuad24.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad11.x, trackQuad11.y, trackQuad11.z ); 
		glVertex3f ( trackQuad21.x, trackQuad21.y, trackQuad21.z ); 
		glVertex3f ( trackQuad12.x, trackQuad12.y, trackQuad12.z ); 
		glVertex3f ( trackQuad22.x, trackQuad22.y, trackQuad22.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad12.x, trackQuad12.y, trackQuad12.z ); 
		glVertex3f ( trackQuad22.x, trackQuad22.y, trackQuad22.z ); 
		glVertex3f ( trackQuad13.x, trackQuad13.y, trackQuad13.z ); 
		glVertex3f ( trackQuad23.x, trackQuad23.y, trackQuad23.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad13.x, trackQuad13.y, trackQuad13.z );
		glVertex3f ( trackQuad14.x, trackQuad14.y, trackQuad14.z ); 
		glVertex3f ( trackQuad23.x, trackQuad23.y, trackQuad23.z ); 
		glVertex3f ( trackQuad24.x, trackQuad24.y, trackQuad24.z );
		glEnd(); */
		/*
		* Render the second track
		*/
		glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x + trackDisplacement1.x, Point1.y + trackDisplacement1.y, Point1.z + trackDisplacement1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x + trackDisplacement2.x, Point2.y + trackDisplacement2.y, Point2.z + trackDisplacement2.z );
		glEnd ();
		/*glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad11.x + trackDisplacement1.x, trackQuad11.y + trackDisplacement1.y, trackQuad11.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad21.x, trackQuad21.y, trackQuad21.z ); 
		glVertex3f ( trackQuad14.x + trackDisplacement1.x, trackQuad14.y, trackQuad14.z ); 
		glVertex3f ( trackQuad24.x, trackQuad24.y, trackQuad24.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad11.x + trackDisplacement1.x, trackQuad11.y + trackDisplacement1.y, trackQuad11.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad21.x + trackDisplacement2.x, trackQuad21.y + trackDisplacement2.y, trackQuad21.z + trackDisplacement2.z ); 
		glVertex3f ( trackQuad12.x + trackDisplacement1.x, trackQuad12.y + trackDisplacement1.y, trackQuad12.z + trackDisplacement2.z ); 
		glVertex3f ( trackQuad22.x + trackDisplacement2.x, trackQuad22.y + trackDisplacement1.y, trackQuad22.z + trackDisplacement2.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad12.x + trackDisplacement1.x, trackQuad12.y + trackDisplacement1.y, trackQuad12.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad22.x + trackDisplacement2.x, trackQuad22.y + trackDisplacement2.y, trackQuad22.z + trackDisplacement2.z ); 
		glVertex3f ( trackQuad13.x + trackDisplacement1.x, trackQuad13.y + trackDisplacement1.y, trackQuad13.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad23.x + trackDisplacement2.x, trackQuad23.y + trackDisplacement2.y, trackQuad23.z + trackDisplacement2.z );
		glEnd();

		glBegin ( GL_QUADS );
		glColor3f ( 1.0, 1.0, 1.0 );  
		glVertex3f ( trackQuad13.x + trackDisplacement1.x, trackQuad13.y + trackDisplacement1.y, trackQuad13.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad14.x + trackDisplacement1.x, trackQuad14.y + trackDisplacement1.y, trackQuad14.z + trackDisplacement1.z ); 
		glVertex3f ( trackQuad23.x + trackDisplacement2.x, trackQuad23.y + trackDisplacement2.y, trackQuad23.z + trackDisplacement2.z ); 
		glVertex3f ( trackQuad24.x + trackDisplacement2.x, trackQuad24.y + trackDisplacement2.y, trackQuad24.z + trackDisplacement2.z );
		glEnd(); */
	}

}

void drawRollerCoaster () {
	for ( index = 0; index < g_Splines[0].numControlPoints - 1; index = index + 1 ) {
		drawSpline ( 0, 1, 0.005 );
	}
}

void drawAxis () {
	glLineWidth (2.0);
	//glPushMatrix ();
	//glLoadIdentity ();
	glBegin ( GL_LINES );
	glColor3f ( 1.0f, 0.0f, 0.0f );
	glVertex3f ( 0.0, 0.0, 0.0 );
	glVertex3f ( 0.5, 0.0, 0.0 );

	glColor3f ( 0.0, 1.0, 0.0 );
	glVertex3f ( 0.0, 0.0, 0.0 );
	glVertex3f ( 0.0, 0.5, 0.0 );

	glColor3f ( 0.0, 0.0, 1.0 );
	glVertex3f ( 0.0, 0.0, 0.0 );
	glVertex3f ( 0.0, 0.0, 0.5 );
	glEnd();
	//glPopMatrix ();
}

void display()
{
	int i, j, k;
	char fileName[50] = "./Screenshots/Screenshot_";
	GLfloat XCoordinate, YCoordinate;
	// Clear the color buffer and the depth buffer
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	//glEnable ( GL_TEXTURE_2D );
	// Load the identity matrix
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();
	glOrtho ( -1.0, 1.0, -1.0/1.0, 1.0/1.0, 0.0, 1.0 );
	//gluLookAt ( 0.0, 0.0, 0.0, 0.0, 1, -1.0, 0, 0, 1 ); // You are at origin and looking at 45 degree angle in YZ direction
	//gluLookAt ( 0.0, 0.0, 0.0, 0.0, 1, 0.0, 0, 0, 1 ); // You are at origin and looking at positive y direction
	//gluLookAt ( 0.5, 1.0, 0.5, 0.5, 0.0, 0.5, 0, 0, 1 ); // You are at (0, 1, 0) and looking at negative y direction
	gluLookAt ( 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 1, 0 ); // You are at (0, 0, 0) and looking at negative z direction
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();
	// Light the scene
	//sceneLighting ();
	glTranslatef ( g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2] );
	// Rotate the image according to the value specified by the g_vLandRotate vector
	glRotatef ( g_vLandRotate[0], 1.0, 0.0, 0.0 ); 
	glRotatef ( g_vLandRotate[1], 0.0, 1.0, 0.0 ); 
	glRotatef ( g_vLandRotate[2], 0.0, 0.0, 1.0 );
	// Scale the image according to the value specified by the g_vLandScale vector
	glScalef ( g_vLandScale[0], g_vLandScale[1], g_vLandScale[2] );

# ifdef SKY
	// Draw the sky dome
	drawSkyDome (1.0, 100, 100);
# else
	//drawCube ();
# endif
	//drawAxis ();
	drawRollerCoaster ();
	/*
	* Whenever the 'c' or 'C' key is pressed on the keybboard the program will start saving the screenshots of the 
	* image being rendered on the screen.
	*/
	if ( startCapture == 1 ) {
		sprintf (fileName, "%s%d%s", fileName, frame, ".jpg" );
		saveScreenshot ( fileName );
		frame = frame + 1;
	}
	// Swap the buffers since we specified double buffering when initializing the Open GL engine
	glutSwapBuffers ();

}

int _tmain(int argc, _TCHAR* argv[])
{
	int i, j, k;
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}
# ifdef SKY
	g_pHeightData = jpeg_read( "blue_sky_1920.jpg" , NULL);
	if (!g_pHeightData)
	{
		printf ("error reading %s.\n", argv[1]);
		exit(1);
	}
# else
	g_pHeightData = jpeg_read( "Cubemap_2_2048x1536.jpg" , NULL);
	if (!g_pHeightData)
	{
		printf ("error reading %s.\n", argv[1]);
		exit(1);
	}
# endif

	printf ( "Read the image file\n" );

	for ( i = 0; i < 1440; i = i + 1 ) {
		for ( j = 0; j < 1920; j = j + 1 ) {
			checkImage[i][j][0] = ( GLubyte ) PIC_PIXEL ( g_pHeightData, j, i, 0);
			checkImage[i][j][1] = ( GLubyte ) PIC_PIXEL ( g_pHeightData, j, i, 1);
			checkImage[i][j][2] = ( GLubyte ) PIC_PIXEL ( g_pHeightData, j, i, 2);
			checkImage[i][j][3] = ( GLubyte ) 255;
		}
	}
	loadSplines(argv[1]);
	glutInit(&argc,(char**)argv);
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(640,480); 
	glutCreateWindow( "CSCI 420 Assignment2" ); 
	glutReshapeFunc( myReshape ); 
	glutDisplayFunc( display );
	glutIdleFunc( doIdle ); 
	glutMouseFunc( mousebutton ); 
	glutKeyboardFunc( keyboard );
	/* callback for mouse drags */
	glutMotionFunc( mousedrag );
	/* callback for idle mouse movement */
	glutPassiveMotionFunc( mouseidle );
	glEnable( GL_DEPTH_TEST );

	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;
}