# include "Ray.h"

Ray::Ray() {
	origin = Vec3(0, 0, 0);
	direction = Vec3(0, 0, -1);
}

Ray::Ray(Vec3 origin, Vec3 direction) {
	this->origin = origin;
	this->direction = direction;
}

void Ray::setOrigin(Vec3 origin) {
	this->origin = origin;
}

void Ray::setDirection(Vec3 direction) {
	this->direction = direction;
}

Vec3 Ray::getOrigin() {
	return origin;
}

Vec3 Ray::getDirection() {
	return direction;
}

double Ray::getZ() {
	return z;
}

