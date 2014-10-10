# include "Vec3.h"

Vec3::Vec3() {
	x = 0;
	y = 0;
	z = 0;
}

Vec3::Vec3(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vec3::setX(double x) {
	this->x = x;
}

void Vec3::setY(double y) {
	this->y = y;
}

void Vec3::setZ(double z) {
	this->z = z;
}

double Vec3::getX() {
	return x;
}

double Vec3::getY() {
	return y;
}

double Vec3::getZ() {
	return z;
}

// Compute the L2 norm of the vector
double Vec3::magnitudeOfVector() {
	return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}

// Normalize the vector
Vec3 Vec3::normalizeVector() {
	double magnitude;

	magnitude = this->magnitudeOfVector();

	if (magnitude != 0) {
		return Vec3(this->x / magnitude, this->y / magnitude, this->z / magnitude);
	}
	else {
		return Vec3(0.0, 0.0, 0.0);
	}
}

// Change the direction of the vector
Vec3 Vec3::invertVector() {
	return Vec3(-this->x, -this->y, -this->z);
}

// Compute the vector dot product
double Vec3::vectorDotProduct(Vec3 vector2) {
	return ((x * vector2.getX()) + (y * vector2.getY()) + (z * vector2.getZ()));
}

// Compute the vector cross product 
Vec3 Vec3::vectorCrossProduct(Vec3 vector2) {
	double xComponent, yComponent, zComponent;

	xComponent = this->y * vector2.getZ() - this->z * vector2.getY();
	yComponent = this->z * vector2.getX() - this->x * vector2.getZ();
	zComponent = this->x * vector2.getY() - this->y * vector2.getX();

	return Vec3(xComponent, yComponent, zComponent);
}

// Compute the vector addition result
Vec3 Vec3::vectorAddition(Vec3 vector2) {
	return Vec3(this->x + vector2.getX(), this->y + vector2.getY(), this->z + vector2.getZ());
}

// Compute the scalar multiplication of the vector
Vec3 Vec3::scalarMultiplication(double c) {
	double xComponent, yComponent, zComponent;
	xComponent = c * this->x;
	yComponent = c * this->y;
	zComponent = c * this->z;

	return Vec3(xComponent, yComponent, zComponent);
}

// Compute the determinant of the vectors
double Vec3::calculateDeterminant(Vec3 vertexB, Vec3 vertexC) {
	double part1, part2, part3;

	part1 = this->getX() * ((vertexB.getY() * vertexC.getZ()) - (vertexB.getZ() * vertexC.getY()));
	part2 = this->getY() * ((vertexB.getX() * vertexC.getZ()) - (vertexB.getZ() * vertexC.getX()));
	part3 = this->getZ() * ((vertexB.getX() * vertexC.getY()) - (vertexB.getY() * vertexC.getX()));

	return (part1 - part2 + part3);
}