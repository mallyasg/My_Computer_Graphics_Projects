# ifndef _VEC3_H
# define _VEC3_H

# include "math.h"

class Vec3 {
	double x;
	double y;
	double z;

public:
	Vec3();
	Vec3(double x, double y, double z);
	void setX(double x);
	void setY(double y);
	void setZ(double z);
	double getX();
	double getY();
	double getZ();
	double magnitudeOfVector();
	Vec3 normalizeVector();
	Vec3 invertVector();
	double vectorDotProduct(Vec3 vector2);
	Vec3 vectorCrossProduct(Vec3 vector2);
	Vec3 vectorAddition(Vec3 vector2);
	Vec3 scalarMultiplication(double c);
	double calculateDeterminant(Vec3 vertexB, Vec3 vertexC);
};

# endif