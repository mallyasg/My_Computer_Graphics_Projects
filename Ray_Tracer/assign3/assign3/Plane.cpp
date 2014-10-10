# include "Plane.h"

Plane::Plane(){
	normal = Vec3(0.0, 0.0, 0.0);
	distanceFromOrigin = 0.0;
	color = Color(1.0, 0.0, 0.0);
}
Plane::Plane(Vec3 normal, double distanceFromOrigin, Color color) {
	this->normal = normal;
	this->distanceFromOrigin = distanceFromOrigin;
	this->color = color;
}
void Plane::setNormal(Vec3 normal) {
	this->normal = normal;
}
void Plane::setdistanceFromOrigin(double distanceFromOrigin) {
	this->distanceFromOrigin = distanceFromOrigin;
}
void Plane::setColor(Color color) {
	this->color = color;
}
Vec3 Plane::getNormal() {
	return this->normal;
}
double Plane::getDistanceFromOrigin() {
	return this->distanceFromOrigin;
}
Color Plane::getPlaneColor() {
	return this->color;
}

Vec3 Plane::getNormalAtIntersection() {
	return this->normal;
}

double Plane::intersectionWithPlane(Ray ray) {
	Vec3 rayDirection, rayOrigin;
	double normalDotDirection = 0, b, normalDotOrigin, nDotOPlusD;

	rayDirection = ray.getDirection();
	rayOrigin = ray.getOrigin();
	normalDotDirection = normal.vectorDotProduct(rayDirection);

	Vec3 pointOfIntersection;


	if (normalDotDirection == 0) {
		// Ray does not intersect
		return NO_INTERSECTION;
	}
	else {
		// If ray intersects obtain the point of intersection using the formula
		// t = (d - n.x0) / n.xd
		normalDotOrigin = normal.vectorDotProduct(rayOrigin);
		nDotOPlusD = -normalDotOrigin + distanceFromOrigin;
		pointOfIntersection = rayOrigin.vectorAddition(rayDirection.scalarMultiplication((nDotOPlusD / normalDotDirection)));
		double dot = pointOfIntersection.vectorDotProduct(normal) - distanceFromOrigin;
		return ((nDotOPlusD / normalDotDirection) - DISTANCE_OFFSET);
	}
}