# include "CameraConfig.h"

CameraConfig::CameraConfig() {
	position = Vec3(0.0, 0.0, 0.0);
	direction = Vec3(0.0, 0.0, -1.0);
	up = Vec3(0.0, 1.0, 0.0);
	right = Vec3(1.0, 0.0, 0.0);
}

CameraConfig::CameraConfig(Vec3 position, Vec3 direction, Vec3 up, Vec3 right) {
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->right = right;
}

void CameraConfig::setCameraPosition(Vec3 position) {
	this->position = position;
}

void CameraConfig::setCameraDirection(Vec3 direction) {
	this->direction = direction;
}

void CameraConfig::setCameraUp(Vec3 up) {
	this->up = up;
}

void CameraConfig::setCameraRight(Vec3 right) {
	this->right = right;
}

Vec3 CameraConfig::getCameraPosition() {
	return position;
}

Vec3 CameraConfig::getCameraDirection() {
	return direction;
}

Vec3 CameraConfig::getCameraUp() {
	return up;
}

Vec3 CameraConfig::getCameraRight() {
	return right;
}