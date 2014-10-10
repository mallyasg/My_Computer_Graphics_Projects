# ifndef _RAY_H
# define _RAY_H

# include "Vec3.h"
# include "Color.h"

class Ray {
	Vec3 origin;
	Vec3 direction;
	Color rgbValue;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction, Color rgb);
	void setOrigin(Vec3 origin);
	void setDirection(Vec3 Direction);
	void setColor(Color rgb);
	Vec3 getOrigin();
	Vec3 getDirection();
	Color getrgbValue();
	Ray computeReflectedRay(Vec3 normalAtIntersection, double scalar);
};

# endif