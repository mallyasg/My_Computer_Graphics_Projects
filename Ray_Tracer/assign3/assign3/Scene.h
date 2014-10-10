# ifndef _SCENE_H
# define _SCENE_H

# include "Color.h"
# include "Ray.h"
# include "Vec3.h"
# include "SphereClass.h"
# include "Plane.h"
# include "LightClass.h"
# include "TriangleClass.h"
# include <vector>
# include <math.h>

# define KS 0.01

# define BACKGROUND_RED 1.0
# define BACKGROUND_GREEN 1.0
# define BACKGROUND_BLUE 1.0

# define SHADOW_RED 0.0
# define SHADOW_GREEN 0.0
# define SHADOW_BLUE 0.0
# define DISTANCE_OFFSET 0.000001
# define KA 1.0
# define NUM_LIGHT_X 5
# define NUM_LIGHT_Z 5

# define SOFT_SHADOW

using namespace std;

class Scene {
	int numOfRebounds;
	int maxRebounds;
public:
	Scene();
	Scene(int numOfRebounds, int maxRebounds);
	void setNumOfRebounds(int numOfRebounds);
	Color getColorOfObject();
	double intersection(Ray ray);
	int findMinimumDistance(vector<double> *intersections);
	int findMinimumDistanceFromShadowRay(vector<double> *intersections, Ray shadowRay, int intersectionObjectIndex);
	Color phongIllumination(Vec3 lightRayForPhong, Vec3 reflectedRayForPhong, Vec3 cameraVectorForPhong, Vec3 normalAtIntersection, Color lightColor, 
		Color diffuseComponent, Color specularComponent, double shininess, double *ambientLight);
	Vec3 computeBaryCentricCoordinates(TriangleClass triangleIndex, Vec3 pointOfIntersection);
	void getColorAtIntersection(vector<SphereClass> *sphereIndex, vector<double> *intersections, vector<TriangleClass> *triangleIndex, vector<LightClass> *lightIndex, 
		Vec3 pointOfIntersection, Vec3 normalAtIntersection, Color *pixelColor, Ray rayFromCamera, int intersectionObjectIndex, double *ambientLight);
	void trace(vector<SphereClass> *sphereIndex, vector<double> *intersections, vector<TriangleClass> *triangleIndex, vector<LightClass> *lightIndex, 
		Ray rayFromCamera, Color *pixelColor, double *ambientLight);
};
#endif