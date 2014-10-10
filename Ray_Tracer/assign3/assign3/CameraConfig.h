# ifndef _CAMERA_CONFIG_H
# define _CAMERA_CONFIG_H

# include "Vec3.h"

class CameraConfig {
	Vec3 position;
	Vec3 direction;
	Vec3 up;
	Vec3 right;
public:
	CameraConfig();
	CameraConfig(Vec3 position, Vec3 direction, Vec3 up, Vec3 right);
	void setCameraPosition(Vec3 position);
	void setCameraDirection(Vec3 direction);
	void setCameraUp(Vec3 up);
	void setCameraRight(Vec3 right);
	Vec3 getCameraPosition();
	Vec3 getCameraDirection();
	Vec3 getCameraUp();
	Vec3 getCameraRight();
};
# endif