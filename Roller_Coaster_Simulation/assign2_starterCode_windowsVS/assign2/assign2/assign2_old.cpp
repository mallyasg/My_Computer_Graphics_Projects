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
# define CUBEWIDTH 50.0
# define NEAR_PLANE 1.0
# define FAR_PLANE 150.0
# define MULT_FACTOR 1.0
# define MAX_LINE_LENGTH 0.05
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
GLint firstTime = 1;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

Pic * g_pHeightData;

static GLubyte checkImage[1920][1440][4];

GLuint sky, negz, posz, negx, posx, negy, posy;
GLuint cubeDisplayListID;
_point *splinePoints = NULL, *tangentPoints = NULL, *normalPoints = NULL, *binormalPoints = NULL;
GLuint splineStoreIndex = 0, numSplinePoints = 0, sceneRendered = 0;

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
		screenWidth = 20.0;
		screenHeight = 20.0 / aspect;
	} else /* aspect > 1 */ { 
		glFrustum ( -10.0/aspect, 10.0/aspect, -10.0, 10.0, -10.0, 10.0 );
		screenWidth = 20.0 / aspect;
		screenHeight = 20.0;
	}
	glMatrixMode(GL_MODELVIEW); 
}

void createTextures ( char *image, GLuint *texId ) {
	g_pHeightData = jpeg_read( image , NULL);
	if (!g_pHeightData)
	{
		printf ("error reading.\n");
		exit(1);
	}

	glEnable( GL_TEXTURE_2D ); 
	glGenTextures( 1, texId ); 

	glBindTexture( GL_TEXTURE_2D, *texId );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPLACE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPLACE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, g_pHeightData->nx, g_pHeightData->ny, GL_RGB, GL_UNSIGNED_BYTE, 
		g_pHeightData->pix);
	glDisable(GL_TEXTURE_2D);
}

void cubeDisplayList ( GLfloat cubeSize ) {

	cubeDisplayListID = glGenLists( 1 );
	glNewList( cubeDisplayListID, GL_COMPILE );
	glDepthMask( GL_TRUE );
	glEnable( GL_TEXTURE_2D );
	GLfloat x = cubeSize, y = cubeSize, z = cubeSize;
	// 1 
	glBindTexture( GL_TEXTURE_2D, posy );
	glBegin( GL_QUADS );
	glColor3f( 1.0, 1.0, 1.0 );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( -x, y, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( -x, y, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( x, y, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( x, y, z );

	glEnd();

	// 2
	glBindTexture( GL_TEXTURE_2D, negy );
	glBegin( GL_QUADS );
	glColor3f( 1.0, 1.0, 1.0 );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, -y, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, -y, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, -y, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, -y, z );

	glEnd();


	//3
	glBindTexture ( GL_TEXTURE_2D, posx );
	glBegin ( GL_QUADS );
	glColor3f ( 1.0, 1.0, 1.0 );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, y, z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( x, y, -z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, -y, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( x, -y, z );

	glEnd();

	// 4
	glBindTexture ( GL_TEXTURE_2D, negx );
	glBegin ( GL_QUADS );
	glColor3f ( 1.0, 1.0, 1.0 );

	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( -x, -y, z ); 
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, -y, -z ); 
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, y, -z ); 
	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( -x, y, z ); 

	glEnd();

	// 5
	glBindTexture ( GL_TEXTURE_2D, posz );
	glBegin ( GL_QUADS );
	glColor3f ( 1.0, 1.0, 1.0 );

	glTexCoord2f( 0.0, 0.0 ); glVertex3f( -x, y, -z );
	glTexCoord2f( 0.0, 1.0 ); glVertex3f( -x, -y, -z );
	glTexCoord2f( 1.0, 1.0 ); glVertex3f( x, -y, -z );
	glTexCoord2f( 1.0, 0.0 ); glVertex3f( x, y, -z );

	glEnd();

	// 6
	glBindTexture ( GL_TEXTURE_2D, negz );
	glBegin ( GL_QUADS );
	glColor3f ( 1.0, 1.0, 1.0 );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, y, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, -y, z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, -y,  z);
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, y, z );

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();
}


void myinit ()
{
	/* setup gl view here */
	/*
	* Set the background color as black and the coloring mode as smooth
	*/
	glClearColor ( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel ( GL_FLAT );
	createTextures ( "negz.jpg", &negz );
	createTextures ( "posz.jpg", &posz );
	createTextures ( "negx.jpg", &negx );
	createTextures ( "posx.jpg", &posx );
	createTextures ( "negy.jpg", &negy );
	createTextures ( "posy.jpg", &posy );

	cubeDisplayList ( CUBEWIDTH );
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

/*void drawCube () {

glDepthMask( GL_TRUE );
glEnable ( GL_TEXTURE_2D );
// Z = 2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace6 );

glTexCoord2f (1.0, 1.0); glVertex3f(-2, -2, 2);
glTexCoord2f (1.0, 0.0); glVertex3f(-2, 2, 2);
glTexCoord2f (0.0, 0.0); glVertex3f(2, 2, 2);
glTexCoord2f (0.0, 1.0); glVertex3f(2, -2, 2);

glEnd();

// Z = -2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace2 );

glTexCoord2f (1.0, 1.0); glVertex3f(2, -2, -2);
glTexCoord2f (1.0, 0.0); glVertex3f(2, 2, -2);
glTexCoord2f (0.0, 0.0); glVertex3f(-2, 2, -2);
glTexCoord2f (0.0, 1.0); glVertex3f(-2, -2, -2);

glEnd();

// X = -2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace1 );

glTexCoord2f (1.0, 1.0); glVertex3f(-2, -2, -2);
glTexCoord2f (1.0, 0.0); glVertex3f(-2, -2, 2);
glTexCoord2f (0.0, 0.0); glVertex3f(-2, 2, 2);
glTexCoord2f (0.0, 1.0); glVertex3f(-2, 2, -2);

glEnd();

// X = 2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace2 );

glTexCoord2f (1.0, 1.0); glVertex3f(2, -2, -2);
glTexCoord2f (1.0, 0.0); glVertex3f(2, -2, 2);
glTexCoord2f (0.0, 0.0); glVertex3f(2, 2, 2);
glTexCoord2f (0.0, 1.0); glVertex3f(2, 2, -2);

glEnd();

// Y = 2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace4 );

glTexCoord2f (1.0, 1.0); glVertex3f(-2, 2, -2);
glTexCoord2f (1.0, 0.0); glVertex3f(-2, 2, 2);
glTexCoord2f (0.0, 0.0); glVertex3f(2, 2, 2);
glTexCoord2f (0.0, 1.0); glVertex3f(2, 2, -2);

glEnd();

// Y = -2
glBegin(GL_QUADS);

glBindTexture ( GL_TEXTURE_2D, cubeFace3 );

glTexCoord2f (1.0, 1.0); glVertex3f(-2, -2, -2);
glTexCoord2f (1.0, 0.0); glVertex3f(-2, -2, 2);
glTexCoord2f (0.0, 0.0); glVertex3f(2, -2, 2);
glTexCoord2f (0.0, 1.0); glVertex3f(2, -2, -2);

glEnd();

glDisable ( GL_TEXTURE_2D );
}
*/
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
			controlMatrix[ i ][ 0 ] =  g_Splines[ 0 ].points[ index - 1 + i ].x * MULT_FACTOR;
			controlMatrix[ i ][ 1 ] =  g_Splines[ 0 ].points[ index - 1 + i ].y * MULT_FACTOR;
			controlMatrix[ i ][ 2 ] =  g_Splines[ 0 ].points[ index - 1 + i ].z * MULT_FACTOR;
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

void normalizeVector ( _point *Vector ) {

	GLfloat magnitudeOfVector = 0.0;

	magnitudeOfVector = sqrt ( ( Vector->x * Vector->x ) + ( Vector->y * Vector->y ) + ( Vector->z * Vector->z ) );
	Vector->x = Vector->x / magnitudeOfVector;
	Vector->y = Vector->y / magnitudeOfVector;
	Vector->z = Vector->z / magnitudeOfVector;
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

		if ( firstTime == 1 ) {

			/*
			* Calculate the tangent vector using the formula t(u) = [3u^2 2u 1 0] M * C
			*/
			tangentPoint1 = findTangent ( u0 );
			tangentVector = findTangent ( u1 );

			normalizeVector ( &tangentPoint1 );
			normalizeVector ( &tangentVector );
			/*
			* Calculate the normal vectors, which is the cross product of any arbitrary vector and the 
			* tangent vector.
			*/
			normalPoint1 = vectorCrossProduct ( arbVector, tangentPoint1 );
			normalVector = vectorCrossProduct ( arbVector, tangentVector );

			/*
			* Normalize the normal vectors
			*/

			normalizeVector ( &normalPoint1 );
			normalizeVector ( &normalVector );

			/*
			* Calculate the bi-normal vectors, which is the cross product of the tangent vector
			* and the normal vector
			*/
			binormalPoint1 = vectorCrossProduct ( tangentPoint1, normalPoint1 );
			binormalVector = vectorCrossProduct ( tangentVector, normalVector );
			/*
			* Normalize the bi-normal vectors
			*/
			normalizeVector ( &binormalPoint1 );
			normalizeVector ( &binormalVector );

		} else {

			tangentPoint1.x = tangentVector.x; tangentPoint1.y = tangentVector.y; tangentPoint1.z = tangentVector.z;
			tangentVector = findTangent ( u1 );
			normalizeVector ( &tangentVector );
			/*
			* Calculate the normal vectors, which is the cross product of any arbitrary vector and the 
			* tangent vector.
			*/
			normalPoint1.x = normalVector.x; normalPoint1.y = normalVector.y; normalPoint1.z = normalVector.z;
			normalVector = vectorCrossProduct ( binormalVector, tangentVector );

			/*
			* Normalize the normal vectors
			*/
			normalizeVector ( &normalVector );

			binormalPoint1.x = binormalVector.x; binormalPoint1.y = binormalVector.y; binormalPoint1.z = binormalVector.z;
			binormalVector = vectorCrossProduct ( tangentVector, normalVector );

			// normalizing the binormal vector
			normalizeVector ( &binormalVector );
		}

		if ( firstTime == 1 ) { 
			firstTime = 0;
		}

		/*
		 * Track displacement is used to give an offset from the current spline to draw the second track
		 */
		trackDisplacement1.x = 0.5 * binormalPoint1.x;
		trackDisplacement1.y = 0.5 * binormalPoint1.y;
		trackDisplacement1.z = 0.5 * binormalPoint1.z;

		trackDisplacement2.x = 0.5 * binormalVector.x;
		trackDisplacement2.y = 0.5 * binormalVector.y;
		trackDisplacement2.z = 0.5 * binormalVector.z;
		/*
		* Compute the four points around the point to draw a cuboid around the spline i.e. to create a cross section
		*/
		trackQuad11.x = Point1.x + 0.001 * ( normalPoint1.x + binormalPoint1.x );  trackQuad11.y = Point1.y + 0.001 * ( normalPoint1.y + binormalPoint1.y );  trackQuad11.z = Point1.z + 0.001 * ( normalPoint1.z + binormalPoint1.z );
		trackQuad12.x = Point1.x + 0.001 * ( -normalPoint1.x + binormalPoint1.x ); trackQuad12.y = Point1.y + 0.001 * ( -normalPoint1.y + binormalPoint1.y ); trackQuad12.z = Point1.z + 0.001 * ( -normalPoint1.z + binormalPoint1.z );
		trackQuad13.x = Point1.x + 0.001 * ( -normalPoint1.x - binormalPoint1.x ); trackQuad13.y = Point1.y + 0.001 * ( -normalPoint1.y - binormalPoint1.y ); trackQuad13.z = Point1.z + 0.001 * ( -normalPoint1.z - binormalPoint1.z );
		trackQuad14.x = Point1.x + 0.001 * ( normalPoint1.x - binormalPoint1.x );  trackQuad14.y = Point1.y + 0.001 * ( normalPoint1.y - binormalPoint1.y );  trackQuad14.z = Point1.z + 0.001 * ( normalPoint1.z - binormalPoint1.z );

		trackQuad21.x = Point2.x + 0.001 * ( normalVector.x + binormalVector.x );  trackQuad21.y = Point2.y + 0.001 * ( normalVector.y + binormalVector.y );  trackQuad21.z = Point2.z + 0.001 * ( normalVector.z + binormalVector.z );
		trackQuad22.x = Point2.x + 0.001 * ( -normalVector.x + binormalVector.x ); trackQuad22.y = Point2.y + 0.001 * ( -normalVector.y + binormalVector.y ); trackQuad22.z = Point2.z + 0.001 * ( -normalVector.z + binormalVector.z );
		trackQuad23.x = Point2.x + 0.001 * ( -normalVector.x - binormalVector.x ); trackQuad23.y = Point2.y + 0.001 * ( -normalVector.y - binormalVector.y ); trackQuad23.z = Point2.z + 0.001 * ( -normalVector.z - binormalVector.z );
		trackQuad24.x = Point2.x + 0.001 * ( normalVector.x - binormalVector.x );  trackQuad24.y = Point2.y + 0.001 * ( normalVector.y - binormalVector.y );  trackQuad24.z = Point2.z + 0.001 * ( normalVector.z - binormalVector.z );
		/*
		* Render the first track
		*/
		glEnable ( GL_DEPTH );

		glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x, Point1.y, Point1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x, Point2.y, Point2.z );
		glEnd ();

		/*
		* Render the second track
		*/
		glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x + trackDisplacement1.x, Point1.y + trackDisplacement1.y, Point1.z + trackDisplacement1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x + trackDisplacement2.x, Point2.y + trackDisplacement2.y, Point2.z + trackDisplacement2.z );
		glEnd ();

		if ( sceneRendered == 0 ) {
			/*
			* Reallocate the memory to the arrays to store the spline points, tangent vectors, normal and binormal vectors
			*/
			splinePoints   = ( _point * ) realloc ( splinePoints  , ( splineStoreIndex + 2 ) * sizeof ( _point ) );
			tangentPoints  = ( _point * ) realloc ( tangentPoints , ( splineStoreIndex + 2 ) * sizeof ( _point ) );
			normalPoints   = ( _point * ) realloc ( normalPoints  , ( splineStoreIndex + 2 ) * sizeof ( _point ) );
			binormalPoints = ( _point * ) realloc ( binormalPoints, ( splineStoreIndex + 2 ) * sizeof ( _point ) );
			/*
			* Store the intermediate spline points to use it for traversal
			*/
			splinePoints[ splineStoreIndex ].x = Point1.x;
			splinePoints[ splineStoreIndex ].y = Point1.y;
			splinePoints[ splineStoreIndex ].z = Point1.z;

			splinePoints[ splineStoreIndex + 1 ].x = Point2.x;
			splinePoints[ splineStoreIndex + 1 ].y = Point2.y;
			splinePoints[ splineStoreIndex + 1 ].z = Point2.z;

			/*
			* Store the intermediate tangent vectors to use it for traversal
			*/
			tangentPoints[ splineStoreIndex ].x = tangentPoint1.x;
			tangentPoints[ splineStoreIndex ].y = tangentPoint1.y;
			tangentPoints[ splineStoreIndex ].z = tangentPoint1.z;

			tangentPoints[ splineStoreIndex + 1 ].x = tangentVector.x;
			tangentPoints[ splineStoreIndex + 1 ].y = tangentVector.y;
			tangentPoints[ splineStoreIndex + 1 ].z = tangentVector.z;

			/*
			* Store the intermediate tangent vectors to use it for traversal
			*/
			normalPoints[ splineStoreIndex ].x = normalPoint1.x;
			normalPoints[ splineStoreIndex ].y = normalPoint1.y;
			normalPoints[ splineStoreIndex ].z = normalPoint1.z;

			normalPoints[ splineStoreIndex + 1 ].x = normalVector.x;
			normalPoints[ splineStoreIndex + 1 ].y = normalVector.y;
			normalPoints[ splineStoreIndex + 1 ].z = normalVector.z;

			/*
			* Store the intermediate binormal vectors to use it for traversal
			*/
			binormalPoints[ splineStoreIndex ].x = binormalPoint1.x;
			binormalPoints[ splineStoreIndex ].y = binormalPoint1.y;
			binormalPoints[ splineStoreIndex ].z = binormalPoint1.z;

			binormalPoints[ splineStoreIndex + 1 ].x = binormalVector.x;
			binormalPoints[ splineStoreIndex + 1 ].y = binormalVector.y;
			binormalPoints[ splineStoreIndex + 1 ].z = binormalVector.z;

			splineStoreIndex = splineStoreIndex + 1;
		}
	}
}

/*
 * This function traverses through the spline points and calls the drawSpline which draws the track recursively.
 */
void drawRollerCoaster () {
	for ( index = 0; index < g_Splines[0].numControlPoints - 1; index = index + 1 ) {
		drawSpline ( 0, 1, MAX_LINE_LENGTH );
	}
	if ( sceneRendered == 0 ) {
		splineStoreIndex = splineStoreIndex + 1;
		numSplinePoints = splineStoreIndex;
		sceneRendered = 1;
	}
}

#ifdef REQUIRED
void moveRollerCoaster ( GLfloat u0, GLfloat u1, GLfloat maxLineLength ) {

	_point eye, center, up;
	_point Point1, Point2, tangentPoint1, tangentPoint2;
	_point normalPoint1, normalPoint2, binormalPoint1, binormalPoint2;
	GLfloat midu, distance;
	_point arbVector;

	arbVector.x = 1;
	arbVector.y = 0;
	arbVector.z = 1;

	eye.x = 0.0;
	eye.y = 0.0;
	eye.z = 0.0;

	center.x = 0.0;
	center.y = 0.0;
	center.z = 0.0;

	up.x = 0.0;
	up.y = 0.0;
	up.z = 0.0;

	midu = ( u0 + u1 ) / 2;	

	Point1 = find3DPoint ( u0 );
	Point2 = find3DPoint ( u1 );

	distance = sqrt ( ( Point1.x - Point2.x ) * ( Point1.x - Point2.x ) + 
		( Point1.y - Point2.y ) * ( Point1.y - Point2.y ) + 
		( Point1.z - Point2.z ) * ( Point1.z - Point2.z ) );

	/*
	* Calculate the distance between the two 3D world points. If the distance comes out to be more than the 
	* maxLineLength value then recursively subdivide the problem. If the distance is less than the maxLineLength 
	* then a line segment is drawn from Point1 to Point2
	*/
	if ( distance > maxLineLength ) {
		moveRollerCoaster ( u0, midu, maxLineLength );
		moveRollerCoaster ( midu, u1, maxLineLength );
	} else {

		if ( firstTime == 1 ) {

			/*
			* Calculate the tangent vector using the formula t(u) = [3u^2 2u 1 0] M * C
			*/
			tangentPoint1 = findTangent ( u0 );
			tangentVector = findTangent ( u1 );

			normalizeVector ( &tangentPoint1 );
			normalizeVector ( &tangentVector );
			/*
			* Calculate the normal vectors, which is the cross product of any arbitrary vector and the 
			* tangent vector.
			*/
			normalPoint1 = vectorCrossProduct ( tangentPoint1, arbVector );
			normalVector = vectorCrossProduct ( tangentVector, arbVector );

			/*
			* Normalize the normal vectors
			*/

			normalizeVector ( &normalPoint1 );
			normalizeVector ( &normalVector );

			/*
			* Calculate the bi-normal vectors, which is the cross product of the tangent vector
			* and the normal vector
			*/
			binormalPoint1 = vectorCrossProduct ( tangentPoint1, normalPoint1 );
			binormalVector = vectorCrossProduct ( tangentVector, normalVector );
			/*
			* Normalize the bi-normal vectors
			*/
			normalizeVector ( &binormalPoint1 );
			normalizeVector ( &binormalVector );

		} else {

			tangentPoint1.x = tangentVector.x; 
			tangentPoint1.y = tangentVector.y; 
			tangentPoint1.z = tangentVector.z;
			tangentVector = findTangent ( u1 );
			normalizeVector ( &tangentVector );
			/*
			* Calculate the normal vectors, which is the cross product of any arbitrary vector and the 
			* tangent vector.
			*/
			normalPoint1.x = normalVector.x; normalPoint1.y = normalVector.y; normalPoint1.z = normalVector.z;
			normalVector = vectorCrossProduct ( binormalVector, tangentVector );

			/*
			* Normalize the normal vectors
			*/
			normalizeVector ( &normalVector );

			binormalPoint1.x = binormalVector.x; 
			binormalPoint1.y = binormalVector.y; 
			binormalPoint1.z = binormalVector.z;
			binormalVector = vectorCrossProduct ( tangentVector, normalVector );

			// normalizing the binormal vector
			normalizeVector ( &binormalVector );
		}

		if ( firstTime == 1 ) { 
			firstTime = 0;
			eye.x = Point1.x;
			eye.y = Point1.y;
			eye.z = Point1.z;

			center.x = tangentPoint1.x + Point1.x;
			center.y = tangentPoint1.y + Point1.y;
			center.z = tangentPoint1.z + Point1.z;

			up.x = normalPoint1.x;
			up.y = normalPoint1.y;
			up.z = normalPoint1.z;

			printf ( "Eye co-ordinates : ( %f, %f, %f )\n", eye.x, eye.y, eye.z );
			printf ( "Center co-ordinates : ( %f, %f, %f )\n", center.x, center.y, center.z );
			printf ( "Up co-ordinates : ( %f, %f, %f )\n", up.x, up.y, up.z );
		}




		glMatrixMode ( GL_PROJECTION );
		glLoadIdentity ();
		glOrtho ( -screenWidth / 2.0, screenWidth / 2.0, -screenHeight / 2.0, screenHeight / 2.0, 1.0, 10.0 );
		//gluLookAt ( eye.x, eye.y, eye.z,  center.x, center.y, center.z, up.x, up.y, up.z );
		gluLookAt ( eye.x, eye.y, eye.z,  center.x, center.y, center.z, up.x, up.y, up.z );
		glMatrixMode ( GL_MODELVIEW );
		glutPostRedisplay();
	}

}
# endif

/*
 * Simulate the effect of motion of roller coaster by using the gluLookAt function by making use of the 
 * spline points, tangent vectors and normal vectors stored while drawing the splines.
 */
void doIdle()
{
	_point eye, center, up;
	if ( sceneRendered == 1 ) {
		if ( splineStoreIndex == numSplinePoints ) {
			splineStoreIndex = 0;
		}
		if ( splineStoreIndex == 2 ) {
			printf ( " Hello\n" );
		}
		eye.x = splinePoints[ splineStoreIndex ].x + 20 * normalPoints[ splineStoreIndex ].x;
		eye.y = splinePoints[ splineStoreIndex ].y + 20 * normalPoints[ splineStoreIndex ].y;
		eye.z = splinePoints[ splineStoreIndex ].z + 20 * normalPoints[ splineStoreIndex ].z;

		center.x = eye.x + tangentPoints[ splineStoreIndex ].x;
		center.y = eye.y + tangentPoints[ splineStoreIndex ].y;
		center.z = eye.z + tangentPoints[ splineStoreIndex ].z;

		up.x = normalPoints[ splineStoreIndex ].x;
		up.y = normalPoints[ splineStoreIndex ].y;
		up.z = normalPoints[ splineStoreIndex ].z;

		splineStoreIndex = splineStoreIndex + 1;
		/*glMatrixMode ( GL_PROJECTION );
		glLoadIdentity ();
		glFrustum ( -screenWidth / 2.0, screenWidth / 2.0, -screenHeight / 2.0, screenHeight / 2.0, NEAR_PLANE, FAR_PLANE );
		gluLookAt ( eye.x, eye.y, eye.z,  center.x, center.y, center.z, up.x, up.y, up.z );
		//gluLookAt ( 0.0, 0.0, 0.0,  0, 0, 1, 0, 1, 0);
		glMatrixMode ( GL_MODELVIEW );*/
	}
	glutPostRedisplay();
}

/*
 * Draw the co-ordinate axes. Red axis the conventional x-axis
 * Green axis is the conventional y-axis and blue axis is the 
 * conventional z-axis
 */
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
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glEnable ( GL_TEXTURE_2D );
	glMatrixMode ( GL_PROJECTION );
		glLoadIdentity ();
		glFrustum ( -screenWidth / 2.0, screenWidth / 2.0, -screenHeight / 2.0, screenHeight / 2.0, NEAR_PLANE, FAR_PLANE );
		//gluLookAt ( eye.x, eye.y, eye.z,  center.x, center.y, center.z, up.x, up.y, up.z );
		gluLookAt ( 0.0, 0.0, 0.0,  0, 0, 1, 0, 1, 0);
		glMatrixMode ( GL_MODELVIEW );
# ifndef REQUIRED
	glTranslatef ( g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2] );
	// Rotate the image according to the value specified by the g_vLandRotate vector
	glRotatef ( g_vLandRotate[0], 1.0, 0.0, 0.0 ); 
	glRotatef ( g_vLandRotate[1], 0.0, 1.0, 0.0 ); 
	glRotatef ( g_vLandRotate[2], 0.0, 0.0, 1.0 );
	// Scale the image according to the value specified by the g_vLandScale vector
	glScalef ( g_vLandScale[0], g_vLandScale[1], g_vLandScale[2] );
# endif

# ifdef SKY
	// Draw the sky dome
	drawSkyDome (1.0, 100, 100);
# else
	glCallList ( cubeDisplayListID );
# endif
	//drawAxis ();
	glDepthMask ( GL_TRUE );
	drawRollerCoaster ();
	glLoadIdentity ();
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
# endif
	//printf ( "Read the image file\n" );

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