// assign1.cpp : Defines the entry point for the console application.
//

/*
CSCI 480 Computer Graphics
Assignment 1: Height Fields
C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>
#pragma warning (disable : 4996)

int g_iMenuId;
int g_iRenderMenuId;
int g_iRotateMenuId;
int g_iHeightMenuId;
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

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

/* vertices of cube about the origin */ 
GLfloat vertices[8][3] = 
{{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, 
{1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0}, 
{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}}; 

/* colors to be assigned to vertices */ 
GLfloat colors[8][3] = 
{{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, 
{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, 
{1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}}; 

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 1.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

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

void face( int a, int b, int c, int d ) 
{ 
	/*
	* Draw the face of the cube with vertices and color as 
	* defined by the indices a, b, c and d.
	*/
	glBegin ( GL_POLYGON ); 
	glColor3fv ( colors[a] ); 
	glVertex3fv ( vertices[a] ); 
	glColor3fv ( colors[b] ); 
	glVertex3fv ( vertices[b] ); 
	glColor3fv ( colors[c] ); 
	glVertex3fv ( vertices[c] ); 
	glColor3fv ( colors[d] ); 
	glVertex3fv ( vertices[d] ); 
	glEnd (); 
} 

void colorcube( void ) 
{ 
	/*
	* Draw the cube by calling the face function repeatedly to
	* draw the faces of the cube.
	*/
	face ( 0,3,2,1 ); 
	face ( 2,3,7,6 ); 
	face ( 0,4,7,3 ); 
	face ( 1,2,6,5 ); 
	face ( 4,5,6,7 ); 
	face ( 0,1,5,4 ); 
} 

void renderAsPoints () {
	/*
	* Render the image pixels as points
	*/
	int i, j, k;
	GLfloat x, y, z, height, width;

	// Obtain the height and width of the image for scaling the image when rendering
	height = ( GLfloat ) g_pHeightData->ny;
	width = ( GLfloat ) g_pHeightData->nx;

	glBegin ( GL_POINTS );
	for ( i = 0; i < height; i = i + 1 ) {
		for ( j = 0; j < width; j = j + 1 ) {
			x = ( GLfloat ) j;
			/*
			* If the zAxis variable is set to zero then the height field is mapped on 
			* the z-axis while the image is mapped on the xy plane.
			* Else if the zAxis variable is set to one then the heigh field is contained
			* in the y-axis and the image is mapped onto the xz plane.
			*/
			if ( zAxis == 0 ) {
				y = ( GLfloat ) i;
			} else {
				z = ( GLfloat ) i;
			} 
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as points by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width) - 0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (x/width)-0.5, y, (z/height) - 0.5);
			} 
		}
	}
	glEnd();

}

void renderAsLines () {
	int i, j, k;
	GLfloat x, y, z, height, width;

	height = ( GLfloat ) g_pHeightData->ny;
	width = ( GLfloat ) g_pHeightData->nx;

	glBegin ( GL_LINES );
	for (i = 0; i < height-1; i = i + 1 ) {
		for ( j = 0; j < width-1; j = j + 1 ) {
			x = ( GLfloat ) j;
			/*
			* If the zAxis variable is set to zero then the height field is mapped on 
			* the z-axis while the image is mapped on the xy plane.
			* Else if the zAxis variable is set to one then the heigh field is contained
			* in the y-axis and the image is mapped onto the xz plane.
			*/
			if ( zAxis == 0 ) {
				y = ( GLfloat ) i;
			} else {
				z = ( GLfloat ) i;
			} 
			// Picking the first vertex in the line 
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width)-0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (x/width)-0.5, y, (z/height)-0.5);
			} 
			// Picking the second vertex in the line
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width)-0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width)-0.5, y, (z/height)-0.5);
			} 
		}
	}
	glEnd();
}

void renderAsTriangles () {
	int i, j, k;
	GLfloat x, y, z, height, width;

	height = ( GLfloat ) g_pHeightData->ny;
	width = ( GLfloat ) g_pHeightData->nx;

	glBegin ( GL_TRIANGLES );
	for (i = 0; i < height-1; i = i + 1 ) {
		for ( j = 0; j < width-1; j = j + 1 ) {
			x = ( GLfloat ) j;
			/*
			* If the zAxis variable is set to zero then the height field is mapped on 
			* the z-axis while the image is mapped on the xy plane.
			* Else if the zAxis variable is set to one then the heigh field is contained
			* in the y-axis and the image is mapped onto the xz plane.
			*/
			if ( zAxis == 0 ) {
				y = ( GLfloat ) i;
			} else {
				z = ( GLfloat ) i;
			} 
			// Picking the first vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width)-0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (x/width)-0.5, y, (z/height)-0.5);
			} 
			// Picking the second vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width)-0.5, (( y + 1.0 )/height)-0.5, z);
			} else {
				glVertex3f ( x/width-0.5, y, ( z + 1.0 )/height-0.5);
			} 
			// Picking the third vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} 
			}
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width)-0.5, (( y + 1.0 )/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width)-0.5, y , (( z + 1.0 )/height)-0.5);
			} 

			// Picking the fourth vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width) - 0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (x/width)-0.5, y, (z/height)-0.5);
			} 
			// Picking the fifth vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width)-0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width)-0.5, y, (z/height)-0.5);
			} 
			// Picking the sixth vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width)-0.5, (( y + 1.0 )/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width)-0.5, y, (( z + 1.0 )/height)-0.5);
			} 
		}
	}
	glEnd();
}

/*
* Unlike in rendering the object as triangle, when the object is being rendered as triangle strip, it is necessary to
* specify only four vertex together in case of repeating vertex.
*/
void renderAsTriangleStrip () {
	int i, j, k;
	GLfloat x, y, z, height, width;

	height = ( GLfloat ) g_pHeightData->ny;
	width = ( GLfloat ) g_pHeightData->nx;

	for (i = 0; i < height-1; i = i + 1 ) {
		for ( j = 0; j < width-1; j = j + 1 ) {
			x = ( GLfloat ) j;
			if ( zAxis == 0 ) {
				y = ( GLfloat ) i;
			} else {
				z = ( GLfloat ) i;
			} 
			glBegin ( GL_TRIANGLE_STRIP );
			// Picking the first vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (x/width)-0.5, (y/height)-0.5, z);
			} else {
				glVertex3f ( (x/width)-0.5, y, (z/height)-0.5);
			} 
			// Picking the second vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width)-0.5, (( y )/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width) - 0.5, y, (( z )/height) - 0.5);
			} 
			// Picking the third vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j, i + 1, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j, i + 1, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x )/width)-0.5, (( y + 1.0 )/height)-0.5, z);
			} else {
				glVertex3f ( (( x )/width)-0.5, y , (( z + 1.0 )/height)-0.5);
			} 

			// Picking the fourth vertex in the triangle mesh for the input image
			/* Picking the color of the pixel if the image is grayscale then the gray scale value 
			* is replicated for all the three channels, else the individual color values are picked
			* from the image and substituted for the corresponding channel.
			*/
			if ( g_pHeightData->bpp == 3 ) {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 );
				if ( zAxis == 0 ) {
					z = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} else {
					y = ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 + PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 1 )/255.0 + 
						PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 2 )/255.0 ) / 3.0;
				} 
			} else {
				glColor3f ( PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0, 
					PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0 );
				if ( zAxis == 0 ) {
					z = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} else {
					y = PIC_PIXEL ( g_pHeightData, j + 1, i + 1, 0 )/255.0;
				} 
			}
			/*
			* Specify the vertices which need to be rendered as a vertex of the line by Open GL
			*/
			if ( zAxis == 0 ) {
				glVertex3f ( (( x + 1.0 )/width) - 0.5, ( ( y + 1.0 )/height)-0.5, z);
			} else {
				glVertex3f ( (( x + 1.0 )/width)-0.5, y, (( z + 1.0 )/height)-0.5);
			}
			glEnd();
		}
	}
}

void drawImage ( void )
{
	/*
	* Depending upon the value of renderValue which is set by the user, the image is rendered either as 
	* Points, Lines, Triangles, Triangle Strips
	*/
	if ( renderValue == 0 ) { 
		renderAsPoints ();
	}
	else if ( renderValue == 1 ) { 
		renderAsLines ();
	} else if ( renderValue == 2 ) {
		renderAsTriangles ();
	}
	else if ( renderValue == 3 ) {
		renderAsTriangleStrip ();
	}

}

void myinit ()
{
	/* setup gl view here */
	/*
	* Set the background color as black and the coloring mode as smooth
	*/
	glClearColor ( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel ( GL_SMOOTH );
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

void display()
{
	int i, j, k;
	char fileName[50] = "./Screenshots/Screenshot_";
	GLfloat XCoordinate, YCoordinate;
	// Clear the color buffer and the depth buffer
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// Load the identity matrix
	glLoadIdentity ();
	// Light the scene
	sceneLighting ();
	// Translate the image according to the value specified by the g_vLandTranslate vector
	glTranslatef ( g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2] );
	// Rotate the image according to the value specified by the g_vLandRotate vector
	glRotatef ( g_vLandRotate[0], 1.0, 0.0, 0.0 ); 
	glRotatef ( g_vLandRotate[1], 0.0, 1.0, 0.0 ); 
	glRotatef ( g_vLandRotate[2], 0.0, 0.0, 1.0 );
	// Scale the image according to the value specified by the g_vLandScale vector
	glScalef ( g_vLandScale[0], g_vLandScale[1], g_vLandScale[2] );

	/*
	* If CUBE is defined then a cube will be rendered by OpenGL instead of the image.
	*/
#ifdef CUBE
	colorcube ();
#else
	drawImage ();
#endif
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

void spinCube ()
{
	/*
	* If the animation flag is set to 1 then the program will start animating the object being rendered to some 
	* pre-defined pattern
	*/
	if ( startAnimation == 1 ) {
		if( startAnimation == 1 )
		{
			// Rotate the angle by one degree in all the directions. If the angle value exceeds 360 degrees set it to 
			// zero so that it starts rotating again freely.
			g_vLandRotate[0] = g_vLandRotate[0] + 1.0;
			if( g_vLandRotate[0] > 360 ) {
				g_vLandRotate[0] = 0;
			}

			g_vLandRotate[1] = g_vLandRotate[1] + 1.0;
			if( g_vLandRotate[1] > 360 ) {
				g_vLandRotate[1] = 0;

			}

			g_vLandRotate[2]= g_vLandRotate[2] + 1.0;
			if( g_vLandRotate[2] > 360 ) {
				g_vLandRotate[2] = 0;
			}

			/* Translate the image in x and y direction by a step of 0.01. As and when the center of the image in x/y 
			* direction crosses the +1 or -1 limit then the object starts translating in the opposite diretion prior 
			* to the before it exceeded the screen dimenstions.
			*/
			g_vLandTranslate[0] = g_vLandTranslate[0] + hStep;
			xCenter = xCenter + hStep;
			if ( (int)xCenter <= -1) {
				hStep = 0.01;
			} else if ( (int)xCenter >= 1) {
				hStep = -0.01;
			}

			g_vLandTranslate[1] = g_vLandTranslate[1] + vStep;
			yCenter = yCenter + vStep;
			if ( ((int)yCenter) <= -1) {
				vStep = 0.01;
			} else if ( ((int)yCenter) >= 1) {
				vStep = -0.01;
			}

		}
	} 
	/* Incase of no-animation then the object being rendered is rotated depending upon the value inputted by the user.
	* If the user presses 
	* 'a' or 'A' - Perform roll 
	* 's' or 'S' - Pitch 
	* 'd' or 'D' -  Yaw
	*/
	else {
		/* spin the cube delta degrees about selected axis */ 
		if ( rotateValue == 0 ) {
			g_vLandRotate[0] += delta;
			if (g_vLandRotate[0] > 360.0) {
				g_vLandRotate[0] -= 360.0; 
			}

		} else if ( rotateValue == 1 ) {
			g_vLandRotate[1] += delta; 
			if (g_vLandRotate[1] > 360.0) {
				g_vLandRotate[1] -= 360.0; 
			}
		} else if ( rotateValue == 2 ) {
			g_vLandRotate[2] += delta; 
			if (g_vLandRotate[2] > 360.0) {
				g_vLandRotate[2] -= 360.0; 
			}
		}
	}

	glutPostRedisplay();

}

/*
* Reset the image being rendered to the default position. This is done by setting all the 
* g_vLandTranslate, g_vLandRotate, g_vLandScale to zero
*/
void resetSettings () {
	g_vLandRotate[0] = 0.0;
	g_vLandRotate[1] = 0.0;
	g_vLandRotate[2] = 0.0;
	g_vLandTranslate[3] = 0.0;
	g_vLandTranslate[3] = 0.0;
	g_vLandTranslate[3] = 0.0;
	g_vLandScale[0] = 0.0;
	g_vLandScale[1] = 0.0;
	g_vLandScale[2] = 0.0;
	rotateValue = 3;
	glutIdleFunc ( spinCube );
	glutPostRedisplay();
}

/*
* This function decodes the option provided when we right click the screen
* 0 - Quit
* 1 - Render the image as Points. Here we first disable the wire frame mode and set the renderValue to 0 so that the 
Open Gl will render the image as points.
* 2 - Render the image as Lines. Here we enable the wire frame mode. Set the renderValue to 1.
* 3 - Render the image as Trianles. Wireframe mode is enabled. Set the renderValue to 2.
* 4 - Render the image as triangle strip. The wire frame mode is disabled before setting the renderValue to 3.
*/
void menufunc(int value)
{
	switch (value)
	{
	case 0:
		exit(0);
		break;
	case 1 : 
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		renderValue = 0;
		break;
	case 2 : 
		glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		renderValue = 1;
		break;
	case 3 :
		glPolygonMode ( GL_FRONT_AND_BACK, GL_TRIANGLES );
		renderValue = 2;
		break;
	case 4 :
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
		renderValue = 3;
		break;
	case 5 : 
		resetSettings ();
		break;
	}
}

/*
* This function decodes the option provided when we right click the screen
* 0 - Quit
* 1 - Render the image as Points. Set the renderValue to 0 so that the 
Open Gl will render the image as points.
* 2 - Render the image as Lines. Set the renderValue to 1.
* 3 - Render the image as Trianles. Set the renderValue to 2.
* 4 - Render the image as triangle strip. Set the renderValue to 3.
*/
void renderMenu ( int value ) {
	switch ( value ) {
	case 0 : 
		renderValue = 0;
		break;
	case 1 : 
		renderValue = 1;
		break;
	case 2 : 
		renderValue = 2;
		break;
	case 3 : 
		renderValue = 3;
		break;
	default :
		renderValue = 2;
		break;
	}
	glutPostRedisplay();
}

/*
* Depending upon the choice selected by the  user different rotation schemes will be implemented
* 0 - Roll
* 1 - Pitch
* 2 - Yaw
*/
void rotateMenu ( int value ) {
	switch ( value ) {
	case 0 : 
		rotateValue = 0;
		break;
	case 1 : 
		rotateValue = 1;
		break;
	case 2 : 
		rotateValue = 2;
		break;
	case 3 :
		rotateValue = 3;
		break;
	default :
		rotateValue = 3;
		break;
	}
	glutIdleFunc(spinCube); 
	glutPostRedisplay();
}

/*
* The height field option gives the flexibility to choose where the height field needs to be mapped
* 0 - The height field will be implemented along the Z-Axis
* 1 - The height field will be mapped to the Y-Axis
*/
void heightMenu ( int value ) {
	switch ( value ) {
	case 0 : 
		zAxis = 1;
		break;
	case 1 : 
		zAxis = 0;
		break;
	default :
		zAxis = 1;
		break;
	}
}

void doIdle()
{
	/* make the screen update */
	glutPostRedisplay();
}

void myReshape( int w, int h ) 
{ 
	GLfloat aspect = (GLfloat) w / (GLfloat) h; 
	glViewport(0, 0, w, h); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	if (w <= h) /* aspect <= 1 */  {
		screenWidth = 4.0;
		screenHeight = 4.0/aspect;
		glFrustum ( -2.0, 2.0, -2.0/aspect, 2.0/aspect, -10.0, 10.0 );
	} else /* aspect > 1 */ { 
		screenWidth = 4.0/aspect;
		screenHeight = 4.0;	
		glFrustum ( -2.0/aspect, 2.0/aspect, -2.0, 2.0, -10.0, 10.0 );
	}
	glMatrixMode(GL_MODELVIEW); 
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

	glutIdleFunc(spinCube); 
	glutPostRedisplay();
	if (stop) {
		glutIdleFunc(NULL); 
	} else {
		glutIdleFunc(spinCube); 
	}
	glutIdleFunc(spinCube); 
	glutPostRedisplay();
} 

int main(int argc, char* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
		printf ("error reading %s.\n", argv[1]);
		exit(1);
	}
	printf( "The dimension of the image is :%d, %d\n", g_pHeightData->nx, g_pHeightData->ny );
	printf ( "The number of bytes per pixel is :%d\n", g_pHeightData->bpp);
	glutInit(&argc,(char**)argv);

	/*
	create a window here..should be double buffered and use depth testing

	the code past here will segfault if you don't have a window set up....
	replace the exit once you add those calls.
	*/
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(640,480); 
	glutCreateWindow("CSCI 420 Assignment1"); 
	glutReshapeFunc(myReshape); 
	glutDisplayFunc(display); 

	// Create a menu for rendering options
	g_iRenderMenuId = glutCreateMenu(renderMenu);
	glutSetMenu(g_iRenderMenuId);//display meuu on right click
	glutAddMenuEntry ( "Render as Points", 0);
	glutAddMenuEntry ( "Render as Lines", 1);
	glutAddMenuEntry ( "Render as Triangles", 2);
	glutAddMenuEntry ( "Render as Triangles Strip", 3);

	// Create a menu for the rotation operations
	g_iRotateMenuId = glutCreateMenu(rotateMenu);
	glutSetMenu(g_iRotateMenuId);//display meuu on right click
	glutAddMenuEntry ( "Roll", 0 );
	glutAddMenuEntry ( "Yaw", 1 );
	glutAddMenuEntry ( "Pitch", 2 );
	glutAddMenuEntry ( "Stop", 3 );

	// Create a menu for the Height field mapping options
	g_iHeightMenuId = glutCreateMenu(heightMenu);
	glutSetMenu(g_iHeightMenuId);//display meuu on right click
	glutAddMenuEntry ( "Z Axis", 0 );
	glutAddMenuEntry ( "Y Axis", 1 );

	// Create the main menu function where the rendering, rotating and the height field menu 
	// options are added as sub-enteries
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);//display meuu on right click
	glutAddMenuEntry("Quit", 0);
	glutAddMenuEntry ("Render as Points ( Press W )", 1);
	glutAddMenuEntry ("Render as Lines ( Press E )", 2);
	glutAddMenuEntry ("Render as Triangles ( Press R )", 3);
	glutAddMenuEntry ("Render as Triangle Strip ( Press T )", 4);
	glutAddSubMenu ("Render Options", g_iRenderMenuId);
	glutAddSubMenu ("Rotate Options", g_iRotateMenuId);
	glutAddSubMenu ("Height Field Options", g_iHeightMenuId);
	glutAddMenuEntry("Capture Screen", 5);
	glutAddMenuEntry("Reset", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	// Assign the spinCube function as the Idle function
	glutIdleFunc(spinCube); 
	glutMouseFunc(mousebutton); 
	glutKeyboardFunc(keyboard);
	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	glEnable(GL_DEPTH_TEST);

	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;
}
