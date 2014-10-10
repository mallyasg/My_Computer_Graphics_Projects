/*
CSCI 480
Assignment 3 Raytracer

Name: <Your name here>
*/
# include "Vec3.h"
# include "Ray.h"
# include "SphereClass.h"
# include "Plane.h"
# include "assign3.h"
# include "Scene.h"
# include "LightClass.h"
# include "TriangleClass.h"

# define SUPERSAMPLING
//# define INITIAL
char *filename = 0;
int mode = MODE_DISPLAY;
unsigned char buffer[HEIGHT][WIDTH][3];

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;
int saveComplete = 0;

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

//MODIFY THIS FUNCTION
void draw_scene()
{
# ifdef INITIAL
	unsigned int x,y;
	//simple output
	for(x=0; x<WIDTH; x++)
	{
		glPointSize(2.0);  
		glBegin(GL_POINTS);
		for(y=0;y < HEIGHT;y++)
		{
			plot_pixel(x,y,x%256,y%256,(x+y)%256);
		}
		glEnd();
		glFlush();
	}
# else 
	vector<double> intersections;
	Vec3 cameraPosition(0.0, 0.0, 0.0), cameraDirection(0.0, 0.0, -1.0);// , cameraUp(0.0, 1.0, 0.0), cameraRight(1.0, 0.0, 0.0);
	Vec3 rayDirection, pixelPosition, invertCameraPosition, pixelPositionPlusDirection;
	//CameraConfig camera(cameraPosition, cameraDirection, cameraUp, cameraRight);
	int i, j, k, superRow, superColumn;
	double aspectRatio = 0.0, xIncrement, yIncrement, xCoordinate, yCoordinate, zCoordinate, tanFOV, intersectionPoint;
	Ray rayFromCamera;
	vector<SphereClass> sphereIndex;
	vector<LightClass> lightIndex;
	vector<TriangleClass> triangleIndex;
	SphereClass dummySphere;
	LightClass dummyLight;
	TriangleClass dummyTriangle;
	Scene scene(0, 100);
	Color pixelColor, superPixelColor;

	//aspectRatio = (int)WIDTH / (int)HEIGHT;
	aspectRatio = 1.33;
	tanFOV = tan(fov * PI / (2.0 * 180.0));

	// Iterate through the sphere objects present in the scene and load it into a vector
	for (k = 0; k < num_spheres; k = k + 1) {
		dummySphere.setSphere(spheres[k]);
		sphereIndex.push_back(dummySphere);
	}

	// Iterate through the triangle objects present in the scene and load it into a vector
	for (k = 0; k < num_triangles; k = k + 1) {
		dummyTriangle.setTriangle(triangles[k]);
		triangleIndex.push_back(dummyTriangle);
		triangleIndex.at(k).setNormalOfPlane();
	}

	// Iterate through all the light objects present in the scene and load it into the vector
	for (k = 0; k < num_lights; k = k + 1) {
		dummyLight.setLight(lights[k]);
		lightIndex.push_back(dummyLight);
	}
	// Shoot rays from the camera position towards the image plane
	for (i = 0; i < HEIGHT; i = i + 1) {
		glPointSize(2.0);
		glBegin(GL_POINTS);
		for (j = 0; j < WIDTH; j = j + 1) {
			// Clear the vector of intersections
			intersections.clear();
			// Clear the pixelColor
			pixelColor.setRed(0.0);
			pixelColor.setGreen(0.0);
			pixelColor.setBlue(0.0);
			superPixelColor.setRed(0.0);
			superPixelColor.setGreen(0.0);
			superPixelColor.setBlue(0.0);
# ifdef SUPERSAMPLING
			// Create extra rays for a single ray or subdivide the pixels into sub-pixels, compute the lighting at each of
			// the sub-pixels. The color of the pixel is the average of the color of the sub-pixels.
			for (superRow = 0; superRow < NUM_SUPER_ROWS; superRow = superRow + 1) {
				for (superColumn = 0; superColumn < NUM_SUPER_COLS; superColumn = superColumn + 1) {
					intersections.clear();
					superPixelColor.setRed(0.0);
					superPixelColor.setGreen(0.0);
					superPixelColor.setBlue(0.0);
# endif
					if (aspectRatio >= 1.0) {
						xIncrement = 2 * aspectRatio * tanFOV / (double)WIDTH;
						yIncrement = 2 * tanFOV / (double)HEIGHT;
					}
					else {
						xIncrement = (2 * tanFOV / (double)WIDTH);
						yIncrement = 2 * aspectRatio * tanFOV / (double)HEIGHT;
					}
# ifdef SUPERSAMPLING
					xIncrement = xIncrement / NUM_SUPER_COLS;
					yIncrement = yIncrement / NUM_SUPER_ROWS;
					xCoordinate = ((NUM_SUPER_COLS * (j - ((double)WIDTH / 2.0))) + superColumn) * xIncrement;
					yCoordinate = (NUM_SUPER_ROWS * (i - ((double)HEIGHT / 2.0)) + superRow) * yIncrement;
# else
					xCoordinate = (j - ((double)WIDTH / 2.0)) * xIncrement;
					yCoordinate = ((HEIGHT - i) - ((double)HEIGHT / 2.0)) * yIncrement;
# endif
					zCoordinate = -1.0;
					pixelPosition.setX(xCoordinate);
					pixelPosition.setY(yCoordinate);
					pixelPosition.setZ(zCoordinate);

					// Set the max
					// Load the origin and direction of the ray being shot
					invertCameraPosition = cameraPosition.invertVector();
					pixelPositionPlusDirection = pixelPosition.vectorAddition(invertCameraPosition);
					rayDirection = pixelPositionPlusDirection.normalizeVector();
					rayFromCamera.setOrigin(cameraPosition);
					rayFromCamera.setDirection(rayDirection);

					// Reset the number of rebounds parameter in the scene class
					scene.setNumOfRebounds(0);
					// Call the trace function to start the ray tracing
# ifdef SUPERSAMPLING
					scene.trace(&sphereIndex, &intersections, &triangleIndex, &lightIndex, rayFromCamera, &superPixelColor, ambient_light);
# else
					scene.trace(&sphereIndex, &intersections, &triangleIndex, &lightIndex, rayFromCamera, &pixelColor, ambient_light);
# endif
# ifdef SUPERSAMPLING
					// Adding the color of the sub-pixels
					pixelColor.setRed(pixelColor.getRed() + superPixelColor.getRed());
					pixelColor.setGreen(pixelColor.getGreen() + superPixelColor.getGreen());
					pixelColor.setBlue(pixelColor.getBlue() + superPixelColor.getBlue());
				}
			}

			// Averaging the color
			pixelColor.setRed(pixelColor.getRed() / (NUM_SUPER_COLS * NUM_SUPER_ROWS));
			pixelColor.setGreen(pixelColor.getGreen() / (NUM_SUPER_COLS * NUM_SUPER_ROWS));
			pixelColor.setBlue(pixelColor.getBlue() / (NUM_SUPER_COLS * NUM_SUPER_ROWS));
# endif

			// If the pixel color exceeds 1.0 clamp it to 1.0
			if (pixelColor.getRed() > 1.0) {
				pixelColor.setRed(1.0);
			}
			if (pixelColor.getGreen() > 1.0) {
				pixelColor.setGreen(1.0);
			}
			if (pixelColor.getBlue() > 1.0) {
				pixelColor.setBlue(1.0);
			}
# ifdef SUPERSAMPLING
			plot_pixel(j, i, pixelColor.getRed() * 255.0, pixelColor.getGreen() * 255.0, pixelColor.getBlue() * 255.0);
# else
			plot_pixel(j, i, pixelColor.getRed() * 255.0, pixelColor.getGreen() * 255.0, pixelColor.getBlue() * 255.0);
# endif
		}
		glEnd();
		glFlush();
	}
# endif
	printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	glColor3f(((double)r) / 256.f, ((double)g) / 256.f, ((double)b) / 256.f);
	glVertex2i(x, y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
# ifdef SUPERSAMPLING
	buffer[HEIGHT - y - 1][x][0] = r;
	buffer[HEIGHT - y - 1][x][1] = g;
	buffer[HEIGHT - y - 1][x][2] = b;
# else
	buffer[y][x][0] = r;
	buffer[y][x][1] = g;
	buffer[y][x][2] = b;
# endif
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	plot_pixel_display(x, y, r, g, b);
	if (mode == MODE_JPEG)
		plot_pixel_jpeg(x, y, r, g, b);
}

void save_jpg()
{
	Pic *in = NULL;

	in = pic_alloc(640, 480, 3, NULL);
	//printf("Saving JPEG file: %s\n", filename);

	memcpy(in->pix, buffer, 3 * WIDTH*HEIGHT);
	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);

}

void parse_check(char *expected, char *found)
{
	if (stricmp(expected, found))
	{
		char error[100];
		printf("Expected '%s ' found '%s '\n", expected, found);
		printf("Parse error, abnormal abortion\n");
		exit(0);
	}

}

void parse_doubles(FILE*file, char *check, double p[3])
{
	char str[100];
	fscanf(file, "%s", str);
	parse_check(check, str);
	fscanf(file, "%lf %lf %lf", &p[0], &p[1], &p[2]);
	printf("%s %lf %lf %lf\n", check, p[0], p[1], p[2]);
}

void parse_rad(FILE*file, double *r)
{
	char str[100];
	fscanf(file, "%s", str);
	parse_check("rad:", str);
	fscanf(file, "%lf", r);
	printf("rad: %f\n", *r);
}

void parse_shi(FILE*file, double *shi)
{
	char s[100];
	fscanf(file, "%s", s);
	parse_check("shi:", s);
	fscanf(file, "%lf", shi);
	printf("shi: %f\n", *shi);
}

int loadScene(char *argv)
{
	FILE *file = fopen(argv, "r");
	int number_of_objects;
	char type[50];
	int i;
	Triangle t;
	Sphere s;
	Light l;
	fscanf(file, "%i", &number_of_objects);

	printf("number of objects: %i\n", number_of_objects);
	char str[200];

	parse_doubles(file, "amb:", ambient_light);

	for (i = 0; i < number_of_objects; i++)
	{
		fscanf(file, "%s\n", type);
		printf("%s\n", type);
		if (stricmp(type, "triangle") == 0)
		{

			printf("found triangle\n");
			int j;

			for (j = 0; j < 3; j++)
			{
				parse_doubles(file, "pos:", t.v[j].position);
				parse_doubles(file, "nor:", t.v[j].normal);
				parse_doubles(file, "dif:", t.v[j].color_diffuse);
				parse_doubles(file, "spe:", t.v[j].color_specular);
				parse_shi(file, &t.v[j].shininess);
			}

			if (num_triangles == MAX_TRIANGLES)
			{
				printf("too many triangles, you should increase MAX_TRIANGLES!\n");
				exit(0);
			}
			triangles[num_triangles++] = t;
		}
		else if (stricmp(type, "sphere") == 0)
		{
			printf("found sphere\n");

			parse_doubles(file, "pos:", s.position);
			parse_rad(file, &s.radius);
			parse_doubles(file, "dif:", s.color_diffuse);
			parse_doubles(file, "spe:", s.color_specular);
			parse_shi(file, &s.shininess);

			if (num_spheres == MAX_SPHERES)
			{
				printf("too many spheres, you should increase MAX_SPHERES!\n");
				exit(0);
			}
			spheres[num_spheres++] = s;
		}
		else if (stricmp(type, "light") == 0)
		{
			printf("found light\n");
			parse_doubles(file, "pos:", l.position);
			parse_doubles(file, "col:", l.color);

			if (num_lights == MAX_LIGHTS)
			{
				printf("too many lights, you should increase MAX_LIGHTS!\n");
				exit(0);
			}
			lights[num_lights++] = l;
		}
		else
		{
			printf("unknown type in scene description:\n%s\n", type);
			exit(0);
		}
	}
	return 0;
}

void display()
{

}

void init()
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, WIDTH, 0, HEIGHT, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
	//hack to make it only draw once
	static int once = 0;
	if (!once)
	{
		draw_scene();
		if (mode == MODE_JPEG)
			save_jpg();
	}
	once = 1;
}

int main(int argc, char ** argv)
{
	if (argc<2 || argc > 3)
	{
		printf("usage: %s <scenefile> [jpegname]\n", argv[0]);
		exit(0);
	}
	if (argc == 3)
	{
		mode = MODE_JPEG;
		filename = argv[2];
	}
	else if (argc == 2)
		mode = MODE_DISPLAY;

	glutInit(&argc, argv);
	loadScene(argv[1]);

	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	int window = glutCreateWindow("Ray Tracer");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
}
