# ifndef _SPHERE_H
# define _SPHERE_H
# include <iostream>
# include "Color.h"
# include "Vec3.h"
# include "Ray.h"

# define NO_INTERSECTION -1
# define INTERSECTS 0
# define DISTANCE_OFFSET 0.000001
typedef struct _Sphere
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double shininess;
	double radius;
} Sphere;

class SphereClass  {
	Sphere sphere;
public:
	SphereClass();
	SphereClass(Sphere sphere);
	void setSphere(Sphere sphere);
	void setSpherePosition(Vec3 position);
	void setSphereDiffuseColor(Color diffuseColor);
	void setSphereSpecularColor(Color specularColor);
	void setRadius(double radius);
	void setSphereShininess(double shininess);
	Sphere getSphere();
	Vec3 getSpherePosition();
	Color getSphereDiffuseColor();
	Color getSphereSpecularColor();
	double getSphereRadius();
	double getSphereShininess();	
	double intersectionWithSphere(Ray ray);
	Vec3 normalAtIntersection(Vec3 intersectionPoint);
};
# endif