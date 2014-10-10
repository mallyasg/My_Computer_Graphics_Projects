# include "assign3.h"
# include "Ray.h"

Ray::Ray() {
	origin = Vec3(0, 0, 0);
	direction = Vec3(0, 0, -1);
	rgbValue = Color(0.0, 0.0, 0.0);
}

Ray::Ray(Vec3 origin, Vec3 direction, Color rgb) {
	this->origin = origin;
	this->direction = direction;
	this->rgbValue = rgb;
}

void Ray::setOrigin(Vec3 origin) {
	this->origin = origin;
}

void Ray::setDirection(Vec3 direction) {
	this->direction = direction;
}

void Ray::setColor(Color rgb) {
	this->rgbValue = rgb;
}

Vec3 Ray::getOrigin() {
	return origin;
}

Vec3 Ray::getDirection() {
	return direction;
}

Color Ray::getrgbValue() {
	return rgbValue;
}
// Compute the reflected ray using the formula r = 2 (l.n) - n
Ray Ray::computeReflectedRay(Vec3 normalAtIntersection, double scalar) {
	Vec3 reflectedRayDirection, incidentRay, origin, direction, scalarTimesNormal, invertDirection, intersectionPoint;
	Vec3 scalarTimesDirection;
	double directionDotNormal;

	// Compute the incident ray
	origin = this->getOrigin();
	direction = this->getDirection();
	scalarTimesDirection = direction.scalarMultiplication(scalar);
	intersectionPoint = origin.vectorAddition(scalarTimesDirection);

	directionDotNormal = direction.vectorDotProduct(normalAtIntersection);
	scalarTimesNormal = normalAtIntersection.scalarMultiplication(2 * directionDotNormal);
	invertDirection = direction.invertVector();
	reflectedRayDirection = scalarTimesNormal.vectorAddition(invertDirection).normalizeVector();

	return Ray(intersectionPoint, reflectedRayDirection, Color(0, 0, 0));
}