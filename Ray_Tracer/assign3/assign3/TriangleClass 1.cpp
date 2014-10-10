# include "TriangleClass.h"

TriangleClass::TriangleClass() {
	int i;

	for (i = 0; i < 3; i = i + 1) {
		triangle.v[i].shininess = 0;
		triangle.v[i].position[0] = 0.0;
		triangle.v[i].position[1] = 0.0;
		triangle.v[i].position[2] = 0.0;
		triangle.v[i].color_diffuse[0] = 0.0;
		triangle.v[i].color_diffuse[1] = 0.0;
		triangle.v[i].color_diffuse[2] = 0.0;
		triangle.v[i].color_specular[0] = 0.0;
		triangle.v[i].color_specular[1] = 0.0;
		triangle.v[i].color_specular[2] = 0.0;
		triangle.v[i].normal[0] = 0.0;
		triangle.v[i].normal[1] = 0.0;
		triangle.v[i].normal[2] = 0.0;
	}
}

TriangleClass::TriangleClass(Triangle triangle) {
	this->triangle = triangle;
}

void TriangleClass::setTriangle(Triangle triangle) {
	this->triangle = triangle;
}

void TriangleClass::setNormalAtVertex0(Vec3 normal) {
	this->triangle.v[0].normal[0] = normal.getX();
	this->triangle.v[0].normal[1] = normal.getY();
	this->triangle.v[0].normal[2] = normal.getZ();
}

void TriangleClass::setNormalAtVertex1(Vec3 normal) {
	this->triangle.v[1].normal[0] = normal.getX();
	this->triangle.v[1].normal[1] = normal.getY();
	this->triangle.v[1].normal[2] = normal.getZ();
}

void TriangleClass::setNormalAtVertex2(Vec3 normal) {
	this->triangle.v[2].normal[0] = normal.getX();
	this->triangle.v[2].normal[1] = normal.getY();
	this->triangle.v[2].normal[2] = normal.getZ();
}

void TriangleClass::setShininessAtVertex0(double shininess) {
	this->triangle.v[0].shininess = shininess;
}

void TriangleClass::setShininessAtVertex1(double shininess) {
	this->triangle.v[1].shininess = shininess;
}

void TriangleClass::setShininessAtVertex2(double shininess) {
	this->triangle.v[2].shininess = shininess;
}

void TriangleClass::setDiffuseColorAtVertex0(Color diffuse) {
	this->triangle.v[0].color_diffuse[0] = diffuse.getRed();
	this->triangle.v[0].color_diffuse[1] = diffuse.getGreen();
	this->triangle.v[0].color_diffuse[2] = diffuse.getBlue();
}

void TriangleClass::setDiffuseColorAtVertex1(Color diffuse) {
	this->triangle.v[1].color_diffuse[0] = diffuse.getRed();
	this->triangle.v[1].color_diffuse[1] = diffuse.getGreen();
	this->triangle.v[1].color_diffuse[2] = diffuse.getBlue();
}

void TriangleClass::setDiffuseColorAtVertex2(Color diffuse) {
	this->triangle.v[2].color_diffuse[0] = diffuse.getRed();
	this->triangle.v[2].color_diffuse[1] = diffuse.getGreen();
	this->triangle.v[2].color_diffuse[2] = diffuse.getBlue();
}

void TriangleClass::setSpecularColorAtVertex0(Color specular) {
	this->triangle.v[0].color_specular[0] = specular.getRed();
	this->triangle.v[0].color_specular[1] = specular.getGreen();
	this->triangle.v[0].color_specular[2] = specular.getBlue();
}

void TriangleClass::setSpecularColorAtVertex1(Color specular) {
	this->triangle.v[1].color_specular[0] = specular.getRed();
	this->triangle.v[1].color_specular[1] = specular.getGreen();
	this->triangle.v[1].color_specular[2] = specular.getBlue();
}

void TriangleClass::setSpecularColorAtVertex2(Color specular) {
	this->triangle.v[2].color_specular[0] = specular.getRed();
	this->triangle.v[2].color_specular[1] = specular.getGreen();
	this->triangle.v[2].color_specular[2] = specular.getBlue();
}

void TriangleClass::setPositionAtVertex0(Vec3 position) {
	this->triangle.v[0].position[0] = position.getX();
	this->triangle.v[0].position[1] = position.getY();
	this->triangle.v[0].position[2] = position.getZ();
}

void TriangleClass::setPositionAtVertex1(Vec3 position) {
	this->triangle.v[1].position[0] = position.getX();
	this->triangle.v[1].position[1] = position.getY();
	this->triangle.v[1].position[2] = position.getZ();
}

void TriangleClass::setPositionAtVertex2(Vec3 position) {
	this->triangle.v[2].position[0] = position.getX();
	this->triangle.v[2].position[1] = position.getY();
	this->triangle.v[2].position[2] = position.getZ();
}

Triangle TriangleClass::getTriangle() {
	return triangle;
}

Vec3 TriangleClass::getNormalAtVertex0() {
	return Vec3(triangle.v[0].normal[0], triangle.v[0].normal[1], triangle.v[0].normal[2]);
}

Vec3 TriangleClass::getNormalAtVertex1() {
	return Vec3(triangle.v[1].normal[0], triangle.v[1].normal[1], triangle.v[1].normal[2]);
}

Vec3 TriangleClass::getNormalAtVertex2() {
	return Vec3(triangle.v[2].normal[0], triangle.v[2].normal[1], triangle.v[2].normal[2]);
}

double TriangleClass::getShininessAtVertex0() {
	return triangle.v[0].shininess;
}

double TriangleClass::getShininessAtVertex1() {
	return triangle.v[1].shininess;
}

double TriangleClass::getShininessAtVertex2() {
	return triangle.v[2].shininess;
}

Color TriangleClass::getDiffuseColorAtVertex0() {
	return Color(triangle.v[0].color_diffuse[0], triangle.v[0].color_diffuse[1], triangle.v[0].color_diffuse[2]);
}

Color TriangleClass::getDiffuseColorAtVertex1() {
	return Color(triangle.v[1].color_diffuse[0], triangle.v[1].color_diffuse[1], triangle.v[1].color_diffuse[2]);
}

Color TriangleClass::getDiffuseColorAtVertex2() {
	return Color(triangle.v[2].color_diffuse[0], triangle.v[2].color_diffuse[1], triangle.v[2].color_diffuse[2]);
}

Color TriangleClass::getSpecularColorAtVertex0() {
	return Color(triangle.v[0].color_specular[0], triangle.v[0].color_specular[1], triangle.v[0].color_specular[2]);
}

Color TriangleClass::getSpecularColorAtVertex1() {
	return Color(triangle.v[1].color_specular[0], triangle.v[1].color_specular[1], triangle.v[1].color_specular[2]);
}

Color TriangleClass::getSpecularColorAtVertex2() {
	return Color(triangle.v[2].color_specular[0], triangle.v[2].color_specular[1], triangle.v[2].color_specular[2]);
}

Vec3 TriangleClass::getPositionAtVertex0() {
	return Vec3(triangle.v[0].position[0], triangle.v[0].position[1], triangle.v[0].position[2]);
}

Vec3 TriangleClass::getPositionAtVertex1() {
	return Vec3(triangle.v[1].position[0], triangle.v[1].position[1], triangle.v[1].position[2]);
}

Vec3 TriangleClass::getPositionAtVertex2() {
	return Vec3(triangle.v[2].position[0], triangle.v[2].position[1], triangle.v[2].position[2]);
}

void TriangleClass::setPositionAtVertex(int vertex, Vec3 position) {
	triangle.v[vertex].position[0] = position.getX();
	triangle.v[vertex].position[1] = position.getY();
	triangle.v[vertex].position[2] = position.getZ();
}

void TriangleClass::setNormalAtVertex(int vertex, Vec3 normal) {
	triangle.v[vertex].normal[0] = normal.getX();
	triangle.v[vertex].normal[1] = normal.getY();
	triangle.v[vertex].normal[2] = normal.getZ();
}

void TriangleClass::setColorDiffuseAtVertex(int vertex, Color colorDiffuse) {
	triangle.v[vertex].color_diffuse[0] = colorDiffuse.getRed();
	triangle.v[vertex].color_diffuse[1] = colorDiffuse.getGreen();
	triangle.v[vertex].color_diffuse[2] = colorDiffuse.getBlue();
}

void TriangleClass::setColorSpecularAtVertex(int vertex, Color colorSpecular) {
	triangle.v[vertex].color_specular[0] = colorSpecular.getRed();
	triangle.v[vertex].color_specular[1] = colorSpecular.getGreen();
	triangle.v[vertex].color_specular[2] = colorSpecular.getBlue();
}

void TriangleClass::setShininessAtVertex(int vertex, double shininess) {
	triangle.v[vertex].shininess = shininess;
}

Vec3 TriangleClass::getPositionAtVertex(int vertex) {
	return Vec3(triangle.v[vertex].position[0], triangle.v[vertex].position[1], triangle.v[vertex].position[2]);
}

Vec3 TriangleClass::getNormalAtVertex(int vertex) {
	return Vec3(triangle.v[vertex].normal[0], triangle.v[vertex].normal[1], triangle.v[vertex].normal[2]);
}

Color TriangleClass::getColorDiffuseAtVertex(int vertex) {
	return Color(triangle.v[vertex].color_diffuse[0], triangle.v[vertex].color_diffuse[1], triangle.v[vertex].color_diffuse[2]);
}

Color TriangleClass::getColorSpecularAtVertex(int vertex) {
	return Color(triangle.v[vertex].color_specular[0], triangle.v[vertex].color_specular[1], triangle.v[vertex].color_specular[2]);
}

double TriangleClass::getShininessAtVertex(int vertex) {
	return triangle.v[vertex].shininess;
}

