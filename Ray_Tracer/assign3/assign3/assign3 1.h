# include <pic.h>
# include <windows.h>
# include <stdlib.h>
# include <GL/glu.h>
# include <GL/glut.h>

# include <stdio.h>
# include <string>
# include <vector>

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2

//you may want to make these smaller for debugging purposes
/*#define WIDTH 640
#define HEIGHT 480*/

#define WIDTH 10
#define HEIGHT 10

//the field of view of the camera
#define fov 60.0

# define PI 3.1415926535
# define NO_INTERSECTION -1
using namespace std;

struct Vertex
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double normal[3];
	double shininess;
};

typedef struct _Triangle
{
	struct Vertex v[3];
} Triangle;



typedef struct _Light
{
	double position[3];
	double color[3];
} Light;