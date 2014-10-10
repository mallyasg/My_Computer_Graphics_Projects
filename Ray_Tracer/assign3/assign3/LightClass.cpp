# include "LightClass.h"

LightClass::LightClass() {
	light.position[0] = 0.0;
	light.position[1] = 0.0;
	light.position[2] = 0.0;
	light.color[0] = 1.0;
	light.color[1] = 1.0;
	light.color[2] = 1.0;
}

LightClass::LightClass(Light light) {
	this->light = light;
}

void LightClass::setLight(Light light) {
	this->light = light;
}

void LightClass::setLightPosition(Vec3 position) {
	light.position[0] = position.getX();
	light.position[1] = position.getY();
	light.position[2] = position.getZ();
}

void LightClass::setLightColor(Color color) {
	light.color[0] = color.getRed();
	light.color[1] = color.getGreen();
	light.color[2] = color.getBlue();
}

Vec3 LightClass::getLightPosition() {
	return Vec3(light.position[0], light.position[1], light.position[2]);
}

Color LightClass::getLightColor() {
	return Color(light.color[0], light.color[1], light.color[2]);
}