# include "SphereClass.h"

SphereClass::SphereClass(){
	sphere.position[0] = 0.0;
	sphere.position[1] = 0.0;
	sphere.position[2] = 0.0;
	sphere.radius = 0.0;
	sphere.color_diffuse[0] = 0.0;
	sphere.color_diffuse[1] = 0.0;
	sphere.color_diffuse[2] = 0.0;
	sphere.color_specular[0] = 0.0;
	sphere.color_specular[1] = 0.0;
	sphere.color_specular[2] = 0.0;
	sphere.shininess = 0.0;
}
SphereClass::SphereClass(Sphere sphere) {
	this->sphere = sphere;
}
void SphereClass::setSphere(Sphere sphere){
	this->sphere = sphere;
}
void SphereClass::setSpherePosition(Vec3 position) {
	sphere.position[0] = position.getX();
	sphere.position[1] = position.getY();
	sphere.position[2] = position.getZ();
}
void SphereClass::setSphereDiffuseColor(Color diffuseColor){
	sphere.color_diffuse[0] = diffuseColor.getRed();
	sphere.color_diffuse[1] = diffuseColor.getGreen();
	sphere.color_diffuse[2] = diffuseColor.getBlue();
}
void SphereClass::setSphereSpecularColor(Color specularColor) {
	sphere.color_specular[0] = specularColor.getRed();
	sphere.color_specular[1] = specularColor.getGreen();
	sphere.color_specular[2] = specularColor.getBlue();
}
void SphereClass::setRadius(double radius) {
	sphere.radius = radius;
}
void SphereClass::setSphereShininess(double shininess){
	sphere.shininess = shininess;
}
Sphere SphereClass::getSphere() {
	return sphere;
}
Vec3 SphereClass::getSpherePosition() {
	return Vec3(sphere.position[0], sphere.position[1], sphere.position[2]);
}
Color SphereClass::getSphereDiffuseColor() {
	return Color(sphere.color_diffuse[0], sphere.color_diffuse[1], sphere.color_diffuse[2]);
}
Color SphereClass::getSphereSpecularColor() {
	return Color(sphere.color_specular[0], sphere.color_specular[1], sphere.color_specular[2]);
}
double SphereClass::getSphereRadius() {
	return sphere.radius;
}
double SphereClass::getSphereShininess() {
	return sphere.shininess;
}

// Obtain the point of intersection with the sphere.
double SphereClass::intersectionWithSphere(Ray ray) {
	Vec3 origin, direction, spherePosition, originMinusCenter;
	double b, c, discriminant, sphereRadius, point1, point2;
	
	origin = ray.getOrigin();
	direction = ray.getDirection();
	spherePosition = this->getSpherePosition();
	originMinusCenter = origin.vectorAddition(spherePosition.invertVector());
	sphereRadius = this->getSphereRadius();

	b = 2 * direction.vectorDotProduct(origin.vectorAddition(spherePosition.invertVector()));
	c = (originMinusCenter.magnitudeOfVector() * originMinusCenter.magnitudeOfVector()) - (sphereRadius * sphereRadius);

	discriminant = (b * b) - (4 * c);

	if (discriminant < 0.0) {
		return NO_INTERSECTION;
	}
	else {
		point1 = ((-b + sqrt(discriminant)) / 2) - DISTANCE_OFFSET;
		point2 = ((-b - sqrt(discriminant)) / 2) - DISTANCE_OFFSET;
		// Return the least among the points of intersection
		if (point2 > 0) {
			return point2;
		}
		else if (point1 > 0) {
			return point1;
		}
		else {
			return NO_INTERSECTION;
		}
	}
}

// Compute the normal at the point of intersection
Vec3 SphereClass::normalAtIntersection(Vec3 intersectionPoint) {
	
	Vec3 normalAtInter;
	normalAtInter = intersectionPoint.vectorAddition(this->getSpherePosition().invertVector());

	return Vec3(normalAtInter.getX() / this->getSphereRadius(), normalAtInter.getY() / this->getSphereRadius(), normalAtInter.getZ() / this->getSphereRadius());
}