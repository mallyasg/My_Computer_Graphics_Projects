# include "Color.h"

Color::Color() {
	red = 0.0;
	green = 0.0;
	blue = 0.0;
}

Color::Color(double r, double g, double b) {
	red = r;
	green = g;
	blue = b;
}

void Color::setRed(double r) {
	red = r;
}

void Color::setGreen(double g) {
	green = g;
}

void Color::setBlue(double b) {
	blue = b;
}

double Color::getRed() {
	return red;
}

double Color::getGreen() {
	return green;
}

double Color::getBlue() {
	return blue;
}