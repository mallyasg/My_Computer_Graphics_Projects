# ifndef _PLANE_H
# define _PLANE_H
# include <iostream>
# include "Color.h"
# include "Vec3.h"
# include "Ray.h"

# define NO_INTERSECTION -1
# define INTERSECTS 0
# define DISTANCE_OFFSET 0.000001

class Plane {
	Vec3 normal;
	double distanceFromOrigin;
	Color color;
public:
	Plane();
	Plane(Vec3 normal, double distanceFromOrigin, Color color);
	void setNormal(Vec3 normal);
	void setdistanceFromOrigin(double distanceFromOrigin);
	void setColor(Color color);
	Vec3 getNormal();
	double getDistanceFromOrigin();
	Color getPlaneColor();
	Vec3 getNormalAtIntersection();
	double intersectionWithPlane(Ray ray);
};
# endif