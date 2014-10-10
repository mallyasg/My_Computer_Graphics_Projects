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
# define FULL
//you may want to make these smaller for debugging purposes

# ifdef FULL
#define WIDTH 640
#define HEIGHT 480
# else
#define WIDTH 10
#define HEIGHT 10
# endif
//the field of view of the camera
#define fov 60.0

# define PI 3.1415926535
# define NO_INTERSECTION -1

# define NUM_SUPER_ROWS 3
# define NUM_SUPER_COLS 3
using namespace std;
