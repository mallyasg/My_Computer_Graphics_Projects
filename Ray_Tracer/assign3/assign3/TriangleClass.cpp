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

Triangle TriangleClass::getTriangle() {
	return triangle;
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

// Compute the normal of the plane containing the triangle, which is the cross product of the two sides of the triangle
void TriangleClass::setNormalOfPlane() {
	Vec3 side1, side2, vertex0, vertex1, vertex2;

	vertex0 = this->getPositionAtVertex(0);
	vertex1 = this->getPositionAtVertex(1);
	vertex2 = this->getPositionAtVertex(2);

	side1 = vertex0.vectorAddition(vertex1.invertVector());
	side2 = vertex2.vectorAddition(vertex1.invertVector());

	normalOfPlane = side1.vectorCrossProduct(side2).normalizeVector();
}

Vec3 TriangleClass::getNormalOfPlane() {
	return normalOfPlane;
}

// Obtain the distance of the plane containing the triangle from the origin
double TriangleClass::getDistanceFromOrigin() {
	double distance = normalOfPlane.vectorDotProduct(this->getPositionAtVertex(1));
	return (normalOfPlane.vectorDotProduct(this->getPositionAtVertex(1)));
}

// Compute the intersection of the ray of light with the triangle
double TriangleClass::intersectionWithTriangle(Ray ray) {

	Plane planeContainingTriangle(this->normalOfPlane, this->getDistanceFromOrigin(), Color(0, 0, 0));
	double intersectionPointWithPlane, areaABC, areaPBC, areaPCA, areaPAB, alpha, beta, gamma;
	Vec3 side1, side2, pointP, origin, direction, vertexA, vertexB, vertexC, vertexP;
	int projectionDone = 0;
	// In order to compute the intersection of the ray with the triangle check whether the ray
	// intersects the plane containing the triangle
	intersectionPointWithPlane = planeContainingTriangle.intersectionWithPlane(ray);

	if (intersectionPointWithPlane == -1) {
		// Ray does not intersect the plane containing the triangle
		return NO_INTERSECTION;
	}
	else {
		origin = ray.getOrigin();
		direction = ray.getDirection();
		pointP = direction.scalarMultiplication(intersectionPointWithPlane).vectorAddition(origin);
# ifdef REQUIRED
		// Project the triangle onto the 2D plane, the 2D plane is selected based on the maximum component of the normal
		if (normalOfPlane.getX() >= normalOfPlane.getY() && normalOfPlane.getX() >= normalOfPlane.getZ() && projectionDone == 0) {
			// Project the triangle onto the X axis
			vertexA.setX(1); vertexA.setY(this->getPositionAtVertex(0).getY()); vertexA.setZ(this->getPositionAtVertex(0).getZ());
			vertexB.setX(1); vertexB.setY(this->getPositionAtVertex(1).getY()); vertexB.setZ(this->getPositionAtVertex(1).getZ());
			vertexC.setX(1); vertexC.setY(this->getPositionAtVertex(2).getY()); vertexC.setZ(this->getPositionAtVertex(2).getZ());
			vertexP.setX(1); vertexP.setY(pointP.getY()); vertexP.setZ(pointP.getZ());
			projectionDone = 1;
		}
		else if (normalOfPlane.getY() >= normalOfPlane.getZ() && normalOfPlane.getY() >= normalOfPlane.getX() && projectionDone == 0) {
			// Project the triangle onto the Y axis
			vertexA.setX(this->getPositionAtVertex(0).getX()); vertexA.setY(1); vertexA.setZ(this->getPositionAtVertex(0).getZ());
			vertexB.setX(this->getPositionAtVertex(1).getX()); vertexB.setY(1); vertexB.setZ(this->getPositionAtVertex(1).getZ());
			vertexC.setX(this->getPositionAtVertex(2).getX()); vertexC.setY(1); vertexC.setZ(this->getPositionAtVertex(2).getZ());
			vertexP.setX(pointP.getX()); vertexP.setY(1); vertexP.setZ(pointP.getZ());
			projectionDone = 1;
		}
		else if (normalOfPlane.getZ() >= normalOfPlane.getX() && normalOfPlane.getZ() >= normalOfPlane.getY() && projectionDone == 0) {
			// Project the triangle onto the Z axis
			vertexA.setX(this->getPositionAtVertex(0).getX()); vertexA.setY(this->getPositionAtVertex(0).getY()); vertexA.setZ(1);
			vertexB.setX(this->getPositionAtVertex(1).getX()); vertexB.setY(this->getPositionAtVertex(1).getY()); vertexB.setZ(1);
			vertexC.setX(this->getPositionAtVertex(2).getX()); vertexC.setY(this->getPositionAtVertex(2).getY()); vertexC.setZ(1);
			vertexP.setX(pointP.getX()); vertexP.setY(pointP.getY()); vertexP.setZ(1);
			projectionDone = 1;
		}
		// Check whether the point of intersection lies within the triangle or not
		// Compute the area of the triangles
		areaABC = vertexA.calculateDeterminant(vertexB, vertexC) / 2.0;

		areaPAB = vertexP.calculateDeterminant(vertexA, vertexB) / 2.0;
		
		areaPBC = vertexP.calculateDeterminant(vertexB, vertexC) / 2.0;

# else 
		// Compute the area of the triangle usign the determinant function
		vertexA.setX(this->getPositionAtVertex(0).getX()); vertexA.setY(this->getPositionAtVertex(0).getY()); vertexA.setZ(this->getPositionAtVertex(0).getZ());
		vertexB.setX(this->getPositionAtVertex(1).getX()); vertexB.setY(this->getPositionAtVertex(1).getY()); vertexB.setZ(this->getPositionAtVertex(1).getZ());
		vertexC.setX(this->getPositionAtVertex(2).getX()); vertexC.setY(this->getPositionAtVertex(2).getY()); vertexC.setZ(this->getPositionAtVertex(2).getZ());
		vertexP.setX(pointP.getX()); vertexP.setY(pointP.getY()); vertexP.setZ(pointP.getZ());

		areaABC = vertexA.calculateDeterminant(vertexB, vertexC);

		areaPAB = vertexP.calculateDeterminant(vertexA, vertexB);

		areaPBC = vertexP.calculateDeterminant(vertexB, vertexC);
# endif
		// Compute the barycentric co-ordinates
		alpha = areaPAB / areaABC;
		beta = areaPBC / areaABC;
		gamma = 1.0 - alpha - beta;

		// If all the bary-centric co-ordinates have the same sign then the point of intersection of the ray with the plane 
		// containing the triangle lies within the triangle
		if ((alpha >= 0.0 && beta >= 0.0 && gamma >= 0.0) || (alpha < 0.0 && beta < 0.0 && gamma < 0.0)) {
			// The point lies inside the triangle
			return intersectionPointWithPlane;
		}
		else {
			// The point lies outside the triangle
			return NO_INTERSECTION;
		}
	}
}