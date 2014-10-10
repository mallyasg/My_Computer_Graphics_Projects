# ifndef _LIGHT_H
# define _LIGHT_H
# include <iostream>
# include "Color.h"
# include "Vec3.h"

typedef struct _Light
{
	double position[3];
	double color[3];
} Light;

class LightClass {
	Light light;
public:
	LightClass();
	LightClass(Light light);
	void setLight(Light light);
	void setLightPosition(Vec3 position);
	void setLightColor(Color color);
	Vec3 getLightPosition();
	Color getLightColor();
};
# endif