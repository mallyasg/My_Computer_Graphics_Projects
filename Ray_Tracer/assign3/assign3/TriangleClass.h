# ifndef _TRIANGLE_H
# define _TRIANGLE_H
# include <iostream>
# include "Color.h"
# include "Vec3.h"
# include "Ray.h"
# include "Plane.h"

# define NO_INTERSECTION -1
# define INTERSECTS 0
# define DISTANCE_OFFSET 0.0001

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


class TriangleClass  {
	Triangle triangle;
	Vec3 normalOfPlane;
public:
	TriangleClass();
	TriangleClass(Triangle triangle);
	void setTriangle(Triangle triangle);
	Triangle getTriangle();
	Vec3 getNormalAtVertex0();
	Vec3 getNormalAtVertex1();
	Vec3 getNormalAtVertex2();
	void setPositionAtVertex(int vertex, Vec3 position);
	void setNormalAtVertex(int vertex, Vec3 normal);
	void setColorDiffuseAtVertex(int vertex, Color colorDiffuse);
	void setColorSpecularAtVertex(int vertex, Color colorSpecular);
	void setShininessAtVertex(int vertex, double shininess);
	Vec3 getPositionAtVertex(int vertex);
	Vec3 getNormalAtVertex(int vertex);
	Color getColorDiffuseAtVertex(int vertex);
	Color getColorSpecularAtVertex(int vertex);
	double getShininessAtVertex(int vertex);
	void setNormalOfPlane();
	Vec3 getNormalOfPlane();
	double getDistanceFromOrigin();
	double intersectionWithTriangle(Ray ray);
};
# endif