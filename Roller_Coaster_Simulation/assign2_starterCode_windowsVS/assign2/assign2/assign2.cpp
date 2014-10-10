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
# define CUBEWIDTH 60.0
# define DOME_RADIUS 150.0
# define NEAR_PLANE 1.0
# define FAR_PLANE 200.0
# define MULT_FACTOR 1.0
# define MAX_LINE_LENGTH 0.1
# define TRACK_WIDTH 0.3
# define SKY
//# define CAPTURE_SCRENSHOT
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
int startPause = 1;

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
GLint lineLengthIndex = 0;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

Pic * g_pHeightData;

static GLubyte checkImage[1920][1440][4];

GLuint sky, negz, posz, negx, posx, negy, posy, woodenBars;
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

/*
 * Create textures by passing the required commands to initialize the texture mapping
 */
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, g_pHeightData->nx, g_pHeightData->ny, GL_RGB, GL_UNSIGNED_BYTE, 
		g_pHeightData->pix);
	glDisable(GL_TEXTURE_2D);
}

/*
 * A display list used in generating the cube map of the environment surrounding the roller coaster
 */
void cubeDisplayList ( GLfloat cubeSize ) {

	cubeDisplayListID = glGenLists( 1 );
	glNewList( cubeDisplayListID, GL_COMPILE );
	glDepthMask( GL_TRUE );
	glEnable( GL_TEXTURE_2D );
	GLfloat x = cubeSize, y = cubeSize, z = cubeSize;
	
	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the positive face of y
	glBindTexture( GL_TEXTURE_2D, posy );
	glBegin( GL_QUADS );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( -x, y, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( -x, y, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( x, y, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( x, y, z );

	glEnd();

	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the negative face of y
	glBindTexture( GL_TEXTURE_2D, negy );
	glBegin( GL_QUADS );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, -y, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, -y, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, -y, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, -y, z );

	glEnd();


	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the positive face of x
	glBindTexture ( GL_TEXTURE_2D, posx );
	glBegin ( GL_QUADS );

	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( x, y, z );
	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, y, -z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, -y, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( x, -y, z );

	glEnd();

	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the negative face of x
	glBindTexture ( GL_TEXTURE_2D, negx );
	glBegin ( GL_QUADS );

	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( -x, -y, z ); 
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, -y, -z ); 
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, y, -z ); 
	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( -x, y, z ); 

	glEnd();

	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the positive face of z
	glBindTexture ( GL_TEXTURE_2D, posz );
	glBegin ( GL_QUADS );

	glTexCoord2f( 0.0, 0.0 ); glVertex3f( -x, y, -z );
	glTexCoord2f( 0.0, 1.0 ); glVertex3f( -x, -y, -z );
	glTexCoord2f( 1.0, 1.0 ); glVertex3f( x, -y, -z );
	glTexCoord2f( 1.0, 0.0 ); glVertex3f( x, y, -z );

	glEnd();

	// Create face of the cube with co-ordinates defined by x, y and z and
	// bind the texture assosciated with the negative face of z
	glBindTexture ( GL_TEXTURE_2D, negz );
	glBegin ( GL_QUADS );

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

	// Call the function which initializes the textures
	createTextures ( "negz.jpg", &negz );
	createTextures ( "posz.jpg", &posz );
	createTextures ( "negx.jpg", &negx );
	createTextures ( "posx.jpg", &posx );
	createTextures ( "negy.jpg", &negy );
	createTextures ( "posy.jpg", &posy );
	createTextures ( "blue_sky_1920.jpg", &sky );
	createTextures ( "wooden_bars.jpg", &woodenBars );
	cubeDisplayList ( CUBEWIDTH );

	/*
	* Define Lighting and Material Properties
	*/
	GLfloat mat_diffuse[4] = { 0.50754, 0.50754, 0.50754, 1.0 };
	GLfloat mat_ambient[4] = { 0.19225, 0.19225, 0.19225, 1.0 };
	GLfloat mat_specular[4] = { 0.508273, 0.508273, 0.508273, 1.0 };
	GLfloat mat_shininess[1] = { 51.2 };
	GLfloat light_position1[4] = {  50.0,  50.0,  50.0, 1.0 };
	GLfloat light_position2[4] = { -50.0, -50.0,  50.0, 1.0 };
	GLfloat light_position3[4] = {  50.0, -50.0,  50.0, 1.0 };
	GLfloat light_position4[4] = { -50.0,  50.0,  50.0, 1.0 };
	GLfloat light_position5[4] = {  50.0, -50.0, -50.0, 1.0 };
	GLfloat light_position6[4] = { -50.0, -50.0, -50.0, 1.0 };
	GLfloat white_light[4] = { 0.5, 0.5, 0.5, 0.0 };
	GLfloat lmodel_ambient[4] = { 0.5, 0.5, 0.5, 1.0 };
	glShadeModel ( GL_SMOOTH );
	glMaterialfv ( GL_FRONT, GL_DIFFUSE, mat_diffuse );
	glMaterialfv ( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialfv ( GL_FRONT, GL_SHININESS, mat_shininess );
	glLightfv ( GL_LIGHT0, GL_POSITION, light_position1 );
	glLightfv ( GL_LIGHT0, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT0, GL_SPECULAR, white_light );
	glLightfv ( GL_LIGHT1, GL_POSITION, light_position2 );
	glLightfv ( GL_LIGHT1, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT1, GL_SPECULAR, white_light );
	glLightfv ( GL_LIGHT2, GL_POSITION, light_position3 );
	glLightfv ( GL_LIGHT2, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT2, GL_SPECULAR, white_light );
	glLightfv ( GL_LIGHT3, GL_POSITION, light_position4 );
	glLightfv ( GL_LIGHT3, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT3, GL_SPECULAR, white_light );
	glLightfv ( GL_LIGHT4, GL_POSITION, light_position5 );
	glLightfv ( GL_LIGHT4, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT4, GL_SPECULAR, white_light );
	glLightfv ( GL_LIGHT5, GL_POSITION, light_position6 );
	glLightfv ( GL_LIGHT5, GL_DIFFUSE, white_light );
	glLightfv ( GL_LIGHT5, GL_SPECULAR, white_light );
	glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, lmodel_ambient );
	// Enable lights from 0 to 6
	glEnable ( GL_LIGHTING );
	glEnable ( GL_LIGHT0 );
	glEnable ( GL_LIGHT1 );
	glEnable ( GL_LIGHT2 );
	glEnable ( GL_LIGHT3 );
	glEnable ( GL_LIGHT4 );
	glEnable ( GL_LIGHT5 );
	glEnable ( GL_DEPTH_TEST );
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
	}

	// Pressing the 'c' key will start the capturing of the images.
	if ( key == 'c' || key == 'C' ) {
		startCapture = ( startCapture == 1 ) ? 0 : 1;
	}

	// Pressing key 'l' will toggle the lighting
	if ( key == 'l' || key == 'L' ) {
		startLighting = ( startLighting == 1 ) ? 0 : 1;
	}
	
	glutPostRedisplay();
	if (stop) {
		glutIdleFunc(NULL); 
	} else {
		
	}
	
	glutPostRedisplay();
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

/*
 * Draw the skydome as hemisphere of quad strips and triangle fan
 */
void drawSkyDome (float radius, int longitudes, int latitudes) {

	GLfloat centerAngle = (( 2 * PI )/360.0) * (1);
	int i, j;
	float XCoordinate, YCoordinate, ZCoordinate;
	float imageX, imageY;
	float incrLongitudes = ( 2 * PI ) / longitudes;
	float incrLatitudes = (( 3 * PI / 2 ) - centerAngle) / (latitudes - 1);
	GLfloat x = radius, y = radius, z = radius;
	//glNormal3f(0, -1, 0);
	glBindTexture ( GL_TEXTURE_2D, sky );
	for ( i = 0; i < longitudes; i = i + 1 ) {
		glBegin(GL_QUAD_STRIP);
		for ( j = 0; j < latitudes; j = j + 1 ) {
			XCoordinate = getXCoordinate ( ( i + 1 ) * incrLongitudes, ( j * incrLatitudes ) + centerAngle, radius );
			YCoordinate = getYCoordinate ( radius, ( j * incrLatitudes) + centerAngle );
			ZCoordinate = getZCoordinate ( ( i + 1) * incrLongitudes, ( j * incrLatitudes ) + centerAngle, radius );
			imageX = ( i + 1 ) * incrLongitudes / ( 2 * PI );
			imageY = ( j * incrLatitudes ) / ( PI / 2 );
			glTexCoord2f( imageX, imageY );
			//glTexCoord2f( XCoordinate, YCoordinate );
			glVertex3f( XCoordinate, YCoordinate, ZCoordinate );

			XCoordinate = getXCoordinate ( i * incrLongitudes, ( j * incrLatitudes ) + centerAngle, radius );
			ZCoordinate = getZCoordinate ( i * incrLongitudes, ( j * incrLatitudes ) + centerAngle, radius );
			imageX = ( i   * incrLongitudes ) / ( 2 * PI );
			imageY = ( j * incrLatitudes ) / ( PI / 2 );
			glTexCoord2f( imageX, imageY );
			//glTexCoord2f ( XCoordinate, YCoordinate );
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
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			// YCoordinate doesn't change
			ZCoordinate = getZCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j + 1 ) and ( i, j )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate(i * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j ) and ( i + 1, j )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, (j * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, (j * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////
			/*
			* ( i + 1, j ) and ( i, j + 1 )
			*/
			XCoordinate = getXCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i+1) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate(i * incrLongitudes, ((j+1) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////

			/*
			* ( i, j + 1 ) and ( i, j )
			*/
			XCoordinate = getXCoordinate((i) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j + 1) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate((i) * incrLongitudes, ((j + 1) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			XCoordinate = getXCoordinate(i * incrLongitudes, ((j) * incrLatitudes) + centerAngle, radius);
			YCoordinate = getYCoordinate(radius, ((j) * incrLatitudes) + centerAngle);
			ZCoordinate = getZCoordinate(i * incrLongitudes, ((j) * incrLatitudes) + centerAngle, radius);
			//glTexCoord2f(YCoordinate / maxY, 0);
			//glTexCoord2f((XCoordinate/radius + 1) / 2, 0);
			glColor3f ( 1.0, 1.0, 1.0 );
			glVertex3f(XCoordinate, YCoordinate, ZCoordinate);

			/////////////////////////////////////////////////////////////////////////////////
		}
		glEnd();
	}
# endif
	/*
	 * Render the top part of the dome as a triangle strip.
	 */
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0);
	glTexCoord2f(1, 0);
	glVertex3f(0, getYCoordinate(radius, 0), 0);
	for (i = 0; i <= longitudes; ++i) {
		XCoordinate = getXCoordinate(i * incrLongitudes, centerAngle, radius);
		YCoordinate = getYCoordinate(radius, centerAngle);
		ZCoordinate = getZCoordinate(i * incrLongitudes, centerAngle, radius);
		//glTexCoord3f( XCoordinate, YCoordinate, ZCoordinate );
		glVertex3f(XCoordinate, YCoordinate, ZCoordinate);
	}
	glEnd();
	glBindTexture( GL_TEXTURE_2D, negy );
	glBegin( GL_QUADS );

	glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( x, 0, z );
	glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( x, 0, -z );
	glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( -x, 0, -z );
	glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( -x, 0, z );

	glEnd();

	glDepthMask( GL_TRUE );
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

		//controlMatrix[ 0 ][ 0 ] =  0.0;
		//controlMatrix[ 0 ][ 1 ] =  0.0;
		//controlMatrix[ 0 ][ 2 ] =  0.0;

		for ( i = 0; i < 4; i = i + 1 ) {
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

	_point Point1, Point2, tangentPoint1, tangentPoint2, track2Point1, track2Point2;
	_point normalPoint1, normalPoint2, binormalPoint1, binormalPoint2;
	GLfloat midu, distance, magNormal, magBinormal;
	_point trackDisplacement1, trackDisplacement2;
	_point arbVector, trackQuad11, trackQuad12, trackQuad13, trackQuad14;
	_point trackQuad21, trackQuad22, trackQuad23, trackQuad24;
	_point woodenQuad1, woodenQuad2, woodenQuad3, woodenQuad4;
	_point pillarQuad1, pillarQuad2, pillarQuad3, pillarQuad4, pillarQuad5, pillarQuad6, pillarQuad7, pillarQuad8;

	arbVector.x = 0;
	arbVector.y = 0;
	arbVector.z = -1;

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
		trackDisplacement1.x = 3.0 * binormalPoint1.x;
		trackDisplacement1.y = 3.0 * binormalPoint1.y;
		trackDisplacement1.z = 3.0 * binormalPoint1.z;

		trackDisplacement2.x = 3.0 * binormalVector.x;
		trackDisplacement2.y = 3.0 * binormalVector.y;
		trackDisplacement2.z = 3.0 * binormalVector.z;

		track2Point1.x = Point1.x + trackDisplacement1.x;
		track2Point1.y = Point1.y + trackDisplacement1.y;
		track2Point1.z = Point1.z + trackDisplacement1.z;

		track2Point2.x = Point2.x + trackDisplacement2.x;
		track2Point2.y = Point2.y + trackDisplacement2.y;
		track2Point2.z = Point2.z + trackDisplacement2.z;
		/*
		* Compute the four points around the point to draw a cuboid around the spline i.e. to create a cross section
		*/
		trackQuad11.x = Point1.x + TRACK_WIDTH * ( normalPoint1.x + binormalPoint1.x );  
		trackQuad11.y = Point1.y + TRACK_WIDTH * ( normalPoint1.y + binormalPoint1.y );  
		trackQuad11.z = Point1.z + TRACK_WIDTH * ( normalPoint1.z + binormalPoint1.z );

		trackQuad12.x = Point1.x + TRACK_WIDTH * ( -normalPoint1.x + binormalPoint1.x ); 
		trackQuad12.y = Point1.y + TRACK_WIDTH * ( -normalPoint1.y + binormalPoint1.y ); 
		trackQuad12.z = Point1.z + TRACK_WIDTH * ( -normalPoint1.z + binormalPoint1.z );

		trackQuad13.x = Point1.x + TRACK_WIDTH * ( -normalPoint1.x - binormalPoint1.x ); 
		trackQuad13.y = Point1.y + TRACK_WIDTH * ( -normalPoint1.y - binormalPoint1.y ); 
		trackQuad13.z = Point1.z + TRACK_WIDTH * ( -normalPoint1.z - binormalPoint1.z );

		trackQuad14.x = Point1.x + TRACK_WIDTH * ( normalPoint1.x - binormalPoint1.x );  
		trackQuad14.y = Point1.y + TRACK_WIDTH * ( normalPoint1.y - binormalPoint1.y );  
		trackQuad14.z = Point1.z + TRACK_WIDTH * ( normalPoint1.z - binormalPoint1.z );

		trackQuad21.x = Point2.x + TRACK_WIDTH * ( normalVector.x + binormalVector.x );  
		trackQuad21.y = Point2.y + TRACK_WIDTH * ( normalVector.y + binormalVector.y );  
		trackQuad21.z = Point2.z + TRACK_WIDTH * ( normalVector.z + binormalVector.z );

		trackQuad22.x = Point2.x + TRACK_WIDTH * ( -normalVector.x + binormalVector.x ); 
		trackQuad22.y = Point2.y + TRACK_WIDTH * ( -normalVector.y + binormalVector.y ); 
		trackQuad22.z = Point2.z + TRACK_WIDTH * ( -normalVector.z + binormalVector.z );

		trackQuad23.x = Point2.x + TRACK_WIDTH * ( -normalVector.x - binormalVector.x ); 
		trackQuad23.y = Point2.y + TRACK_WIDTH * ( -normalVector.y - binormalVector.y ); 
		trackQuad23.z = Point2.z + TRACK_WIDTH * ( -normalVector.z - binormalVector.z );

		trackQuad24.x = Point2.x + TRACK_WIDTH * ( normalVector.x - binormalVector.x );  
		trackQuad24.y = Point2.y + TRACK_WIDTH * ( normalVector.y - binormalVector.y );  
		trackQuad24.z = Point2.z + TRACK_WIDTH * ( normalVector.z - binormalVector.z );
		/*
		* Render the first track
		*/
		glEnable ( GL_DEPTH );

		/*glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x, Point1.y, Point1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x, Point2.y, Point2.z );
		glEnd ();*/
		glColor3f ( 1.0, 1.0, 1.0 );
		glBegin ( GL_QUADS );
		glNormal3f ( -binormalPoint1.x, -binormalPoint1.y, -binormalPoint1.z ); 
		glVertex3f ( trackQuad14.x, trackQuad14.y, trackQuad14.z );
		glNormal3f ( normalPoint1.x, normalPoint1.y, normalPoint1.z ); 
		glVertex3f ( trackQuad11.x, trackQuad11.y, trackQuad11.z );
		glNormal3f ( normalVector.x, normalVector.y, normalVector.z ); 
		glVertex3f ( trackQuad21.x, trackQuad21.y, trackQuad21.z );
		glNormal3f ( -binormalVector.x, -binormalVector.y, -binormalVector.z ); 
		glVertex3f ( trackQuad24.x, trackQuad24.y, trackQuad24.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( normalPoint1.x, normalPoint1.y, normalPoint1.z );
		glVertex3f ( trackQuad11.x, trackQuad11.y, trackQuad11.z );
		glNormal3f ( -binormalPoint1.x, -binormalPoint1.y, -binormalPoint1.z ); 
		glVertex3f ( trackQuad12.x, trackQuad12.y, trackQuad12.z );
		glNormal3f ( -binormalVector.x, -binormalVector.y, -binormalVector.z ); 
		glVertex3f ( trackQuad22.x, trackQuad22.y, trackQuad22.z );
		glNormal3f ( normalVector.x, normalVector.y, normalVector.z ); 
		glVertex3f ( trackQuad21.x, trackQuad21.y, trackQuad21.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( -binormalPoint1.x, -binormalPoint1.y, -binormalPoint1.z ); 
		glVertex3f ( trackQuad14.x, trackQuad14.y, trackQuad14.z );
		glNormal3f ( -normalPoint1.x, -normalPoint1.y, -normalPoint1.z ); 
		glVertex3f ( trackQuad13.x, trackQuad13.y, trackQuad13.z );
		glNormal3f ( -normalVector.x, -normalVector.y, -normalVector.z ); 
		glVertex3f ( trackQuad23.x, trackQuad23.y, trackQuad23.z );
		glNormal3f ( -binormalVector.x, -binormalVector.y, -binormalVector.z ); 
		glVertex3f ( trackQuad24.x, trackQuad24.y, trackQuad24.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( -normalPoint1.x, -normalPoint1.y, -normalPoint1.z );
		glVertex3f ( trackQuad13.x, trackQuad13.y, trackQuad13.z );
		glNormal3f ( binormalPoint1.x, binormalPoint1.y, binormalPoint1.z );
		glVertex3f ( trackQuad12.x, trackQuad12.y, trackQuad12.z );
		glNormal3f ( binormalVector.x, binormalVector.y, binormalVector.z );
		glVertex3f ( trackQuad22.x, trackQuad22.y, trackQuad22.z );
		glNormal3f ( -normalVector.x, -normalVector.y, -normalVector.z );
		glVertex3f ( trackQuad23.x, trackQuad23.y, trackQuad23.z );
		glEnd();
		
		/*glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 );
		glVertex3f ( Point1.x, Point1.y, Point1.z);
		glColor3f ( 1.0, 1.0, 1.0 );
		glVertex3f ( track2Point1.x, track2Point1.y, track2Point1.z );
		glEnd ();*/
		/*
		* Render the second track
		*/
		/*glBegin ( GL_LINES );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point1.x + trackDisplacement1.x, Point1.y + trackDisplacement1.y, Point1.z + trackDisplacement1.z );
		glColor3f ( 1.0, 1.0, 1.0 ); glVertex3f ( Point2.x + trackDisplacement2.x, Point2.y + trackDisplacement2.y, Point2.z + trackDisplacement2.z );
		glEnd ();*/
		
		glBegin ( GL_QUADS );
		glNormal3f ( normalPoint1.x, normalPoint1.y, normalPoint1.z ); 
		glVertex3f ( trackQuad14.x + trackDisplacement1.x, trackQuad14.y + trackDisplacement1.y, trackQuad14.z + trackDisplacement1.z );
		glNormal3f ( normalPoint1.x, normalPoint1.y, normalPoint1.z ); 
		glVertex3f ( trackQuad11.x + trackDisplacement1.x, trackQuad11.y + trackDisplacement1.y, trackQuad11.z + trackDisplacement1.z );
		glNormal3f ( normalVector.x, normalVector.y, normalVector.z );
		glVertex3f ( trackQuad21.x + trackDisplacement2.x, trackQuad21.y + trackDisplacement2.y, trackQuad21.z + trackDisplacement2.z );
		glNormal3f ( normalVector.x, normalVector.y, normalVector.z );
		glVertex3f ( trackQuad24.x + trackDisplacement2.x, trackQuad24.y + trackDisplacement2.y, trackQuad24.z + trackDisplacement2.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( binormalPoint1.x, binormalPoint1.y, binormalPoint1.z ); 
		glVertex3f ( trackQuad11.x + trackDisplacement1.x, trackQuad11.y + trackDisplacement1.y, trackQuad11.z + trackDisplacement1.z );
		glNormal3f ( binormalPoint1.x, binormalPoint1.y, binormalPoint1.z ); 
		glVertex3f ( trackQuad12.x + trackDisplacement1.x, trackQuad12.y + trackDisplacement1.y, trackQuad12.z + trackDisplacement1.z );
		glNormal3f ( binormalVector.x, binormalVector.y, binormalVector.z );
		glVertex3f ( trackQuad22.x + trackDisplacement2.x, trackQuad22.y + trackDisplacement2.y, trackQuad22.z + trackDisplacement2.z );
		glNormal3f ( binormalVector.x, binormalVector.y, binormalVector.z );
		glVertex3f ( trackQuad21.x + trackDisplacement2.x, trackQuad21.y + trackDisplacement2.y, trackQuad21.z + trackDisplacement2.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( -binormalPoint1.x, -binormalPoint1.y, -binormalPoint1.z );
		glVertex3f ( trackQuad14.x + trackDisplacement1.x, trackQuad14.y + trackDisplacement1.y, trackQuad14.z + trackDisplacement1.z );
		glNormal3f ( -binormalPoint1.x, -binormalPoint1.y, -binormalPoint1.z ); 
		glVertex3f ( trackQuad13.x + trackDisplacement1.x, trackQuad13.y + trackDisplacement1.y, trackQuad13.z + trackDisplacement1.z );
		glNormal3f ( -binormalVector.x, -binormalVector.y, -binormalVector.z );
		glVertex3f ( trackQuad23.x + trackDisplacement2.x, trackQuad23.y + trackDisplacement2.y, trackQuad23.z + trackDisplacement2.z );
		glNormal3f ( -binormalVector.x, -binormalVector.y, -binormalVector.z ); 
		glVertex3f ( trackQuad24.x + trackDisplacement2.x, trackQuad24.y + trackDisplacement2.y, trackQuad24.z + trackDisplacement2.z );
		glEnd();

		glBegin ( GL_QUADS );
		glNormal3f ( -normalPoint1.x, -normalPoint1.y, -normalPoint1.z );
		glVertex3f ( trackQuad13.x + trackDisplacement1.x, trackQuad13.y + trackDisplacement1.y, trackQuad13.z + trackDisplacement1.z );
		glNormal3f ( -normalPoint1.x, -normalPoint1.y, -normalPoint1.z );
		glVertex3f ( trackQuad12.x + trackDisplacement1.x, trackQuad12.y + trackDisplacement1.y, trackQuad12.z + trackDisplacement1.z );
		glNormal3f ( -normalVector.x, -normalVector.y, -normalVector.z );
		glVertex3f ( trackQuad22.x + trackDisplacement2.x, trackQuad22.y + trackDisplacement2.y, trackQuad22.z + trackDisplacement2.z );
		glNormal3f ( -normalVector.x, -normalVector.y, -normalVector.z );
		glVertex3f ( trackQuad23.x + trackDisplacement2.x, trackQuad23.y + trackDisplacement2.y, trackQuad23.z + trackDisplacement2.z );
		glEnd();
		
		/*
		* Render the wooden cross bars
		*/
		
		woodenQuad1.x = trackQuad13.x + ( ( 0.1 * tangentPoint1.x ) );
		woodenQuad1.y = trackQuad13.y + ( ( 0.1 * tangentPoint1.y ) );
		woodenQuad1.z = trackQuad13.z + ( ( 0.1 * tangentPoint1.z ) );

		woodenQuad2.x = trackQuad13.x + ( ( 0.1 * -tangentPoint1.x ) );
		woodenQuad2.y = trackQuad13.y + ( ( 0.1 * -tangentPoint1.y ) );
		woodenQuad2.z = trackQuad13.z + ( ( 0.1 * -tangentPoint1.z ) );

		woodenQuad3.x = trackQuad12.x + trackDisplacement1.x + ( ( 0.1 * tangentPoint1.x ) );
		woodenQuad3.y = trackQuad12.y + trackDisplacement1.y + ( ( 0.1 * tangentPoint1.y ) );
		woodenQuad3.z = trackQuad12.z + trackDisplacement1.z + ( ( 0.1 * tangentPoint1.z ) );

		woodenQuad4.x = trackQuad12.x + trackDisplacement1.x + ( ( 0.1 * -tangentPoint1.x ) );
		woodenQuad4.y = trackQuad12.y + trackDisplacement1.y + ( ( 0.1 * -tangentPoint1.y ) );
		woodenQuad4.z = trackQuad12.z + trackDisplacement1.z + ( ( 0.1 * -tangentPoint1.z ) );

		if ( lineLengthIndex % 10 == 0 ) {
			glDepthMask( GL_TRUE );
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, woodenBars );
			glBegin ( GL_QUADS );
			glTexCoord2f ( 0.0, 0.0 ); glVertex3f ( woodenQuad1.x, woodenQuad1.y, woodenQuad1.z );
			glTexCoord2f ( 1.0, 0.0 ); glVertex3f ( woodenQuad3.x, woodenQuad3.y, woodenQuad3.z );
			glTexCoord2f ( 1.0, 1.0 ); glVertex3f ( woodenQuad4.x, woodenQuad4.y, woodenQuad4.z );
			glTexCoord2f ( 0.0, 1.0 ); glVertex3f ( woodenQuad2.x, woodenQuad2.y, woodenQuad2.z );
			glEnd();
			glDisable( GL_TEXTURE_2D );
			/*glBegin ( GL_LINES );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( trackQuad13.x, trackQuad13.y, trackQuad13.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( trackQuad12.x + trackDisplacement1.x, trackQuad12.y + trackDisplacement1.y, trackQuad12.z + trackDisplacement1.z );
			glEnd ();*/
		}
		lineLengthIndex = lineLengthIndex + 1;
		
		/*
		* Render the support pillars
		*/

		pillarQuad1.x = Point1.x + ( -binormalPoint1.x );
		pillarQuad1.y = Point1.y + ( -binormalPoint1.y );
		pillarQuad1.z = Point1.z + ( -binormalPoint1.z );

		pillarQuad2.x = Point1.x + ( -binormalPoint1.x );
		pillarQuad2.y = 0.0;
		pillarQuad2.z = Point1.z + ( -binormalPoint1.z );

		pillarQuad3.x = Point1.x + ( binormalPoint1.x );
		pillarQuad3.y = 0.0;
		pillarQuad3.z = Point1.z + ( binormalPoint1.z );

		pillarQuad4.x = Point1.x + ( binormalPoint1.x );
		pillarQuad4.y = Point1.y + ( binormalPoint1.y );
		pillarQuad4.z = Point1.z + ( binormalPoint1.z );

		pillarQuad5.x = Point1.x + tangentPoint1.x + ( -binormalPoint1.x );
		pillarQuad5.y = Point1.y + tangentPoint1.y + ( -binormalPoint1.y );
		pillarQuad5.z = Point1.z + tangentPoint1.z + ( -binormalPoint1.z );

		pillarQuad6.x = Point1.x + tangentPoint1.x + ( -binormalPoint1.x );
		pillarQuad6.y = 0.0;
		pillarQuad6.z = Point1.z + tangentPoint1.z + ( -binormalPoint1.z );

		pillarQuad7.x = Point1.x + tangentPoint1.x + ( binormalPoint1.x );
		pillarQuad7.y = 0.0;
		pillarQuad7.z = Point1.z + tangentPoint1.z + ( binormalPoint1.z );

		pillarQuad8.x = Point1.x + tangentPoint1.x + ( binormalPoint1.x );
		pillarQuad8.y = Point1.y + tangentPoint1.y + ( binormalPoint1.y );
		pillarQuad8.z = Point1.z + tangentPoint1.z + ( binormalPoint1.z );

		/*if ( lineLengthIndex % 20 == 0 ) {
			glDepthMask( GL_TRUE );

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad1.x, pillarQuad1.y, pillarQuad1.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad2.x, pillarQuad2.y, pillarQuad2.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad3.x, pillarQuad3.y, pillarQuad3.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad4.x, pillarQuad4.y, pillarQuad4.z );
			glEnd ();

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad5.x, pillarQuad5.y, pillarQuad5.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad1.x, pillarQuad1.y, pillarQuad1.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad4.x, pillarQuad4.y, pillarQuad4.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad8.x, pillarQuad8.y, pillarQuad8.z );
			glEnd ();

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad5.x, pillarQuad5.y, pillarQuad5.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad6.x, pillarQuad6.y, pillarQuad6.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad7.x, pillarQuad7.y, pillarQuad7.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad8.x, pillarQuad8.y, pillarQuad8.z );
			glEnd ();

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad6.x, pillarQuad6.y, pillarQuad6.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad2.x, pillarQuad2.y, pillarQuad2.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad3.x, pillarQuad3.y, pillarQuad3.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad7.x, pillarQuad7.y, pillarQuad7.z );
			glEnd ();

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad5.x, pillarQuad5.y, pillarQuad5.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad6.x, pillarQuad6.y, pillarQuad6.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad2.x, pillarQuad2.y, pillarQuad2.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad1.x, pillarQuad1.y, pillarQuad1.z );
			glEnd ();

			glBegin ( GL_QUADS );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad4.x, pillarQuad4.y, pillarQuad4.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad3.x, pillarQuad3.y, pillarQuad3.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad7.x, pillarQuad7.y, pillarQuad7.z );
			glColor3f ( 1.0, 0.0, 0.0 ); glVertex3f ( pillarQuad8.x, pillarQuad8.y, pillarQuad8.z );
			glEnd ();

		}*/
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
			splinePoints[ splineStoreIndex ].x = Point1.x; //( 2 * Point1.x + trackDisplacement1.x ) / 2.0;
			splinePoints[ splineStoreIndex ].y = Point1.y; //( 2 * Point1.y + trackDisplacement1.y ) / 2.0;
			splinePoints[ splineStoreIndex ].z = Point1.z; //( 2 * Point1.z + trackDisplacement1.z ) / 2.0;

			splinePoints[ splineStoreIndex + 1 ].x = Point1.x; //( 2 * Point2.x + trackDisplacement2.x ) / 2.0;
			splinePoints[ splineStoreIndex + 1 ].y = Point1.y; //( 2 * Point2.y + trackDisplacement2.y ) / 2.0;
			splinePoints[ splineStoreIndex + 1 ].z = Point1.z; //( 2 * Point2.z + trackDisplacement2.z ) / 2.0;

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
	for ( index = 0, lineLengthIndex = 0; index < g_Splines[0].numControlPoints - 1; index = index + 1 ) {
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
	char fileName[50] = "./Screenshots/Screenshot_";

	if ( sceneRendered == 1 ) {
		if ( splineStoreIndex == numSplinePoints ) {
			splineStoreIndex = 0;
		}
		if ( startPause == 1 ) {
			eye.x = splinePoints[ splineStoreIndex ].x + 10 * normalPoints[ splineStoreIndex ].x;
			eye.y = splinePoints[ splineStoreIndex ].y + 10 * normalPoints[ splineStoreIndex ].y;
			eye.z = splinePoints[ splineStoreIndex ].z + 10 * normalPoints[ splineStoreIndex ].z;

			center.x = eye.x + tangentPoints[ splineStoreIndex ].x;
			center.y = eye.y + tangentPoints[ splineStoreIndex ].y;
			center.z = eye.z + tangentPoints[ splineStoreIndex ].z;

			up.x = normalPoints[ splineStoreIndex ].x;
			up.y = normalPoints[ splineStoreIndex ].y;
			up.z = normalPoints[ splineStoreIndex ].z;

			splineStoreIndex = splineStoreIndex + 1;

		}
		glMatrixMode ( GL_PROJECTION );
		glLoadIdentity ();
		glFrustum ( -screenWidth / 2.0, screenWidth / 2.0, -screenHeight / 2.0, screenHeight / 2.0, NEAR_PLANE, FAR_PLANE );
		gluLookAt ( eye.x, eye.y, eye.z,  center.x, center.y, center.z, up.x, up.y, up.z );
		//gluLookAt ( 0.0, 0.0, 0.0,  1, 0, 0, 0, 1, 0);
		glMatrixMode ( GL_MODELVIEW );
	}
	glutPostRedisplay();
	//if ( startCapture == 1 ) {
# ifdef CAPTURE_SCRENSHOT
		sprintf (fileName, "%s%d%s", fileName, frame, ".jpg" );
		saveScreenshot ( fileName );
		frame = frame + 1;
# endif
	//}
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

# ifdef REQUIRED
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
	drawSkyDome ( DOME_RADIUS, 100, 100 );
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
	
	//glutPostRedisplay ();
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