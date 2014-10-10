# include "Scene.h"

Scene::Scene() {
	numOfRebounds = 0;
	maxRebounds = 25;
}

Scene::Scene(int numOfRebounds, int maxRebounds) {
	this->numOfRebounds = numOfRebounds;
	this->maxRebounds = maxRebounds;
}

Color Scene::getColorOfObject() {
	return Color(0.0, 0.0, 0.0);
}

void Scene::setNumOfRebounds(int numOfRebounds) {
	this->numOfRebounds = numOfRebounds;
}
double Scene::intersection(Ray ray) {
	return 0.0;
}

int Scene::findMinimumDistance(vector<double> *intersections) {
	int k, nearestObjectIndex = 0;
	double minimumDistance;

	if (intersections->size() == 0) {
		nearestObjectIndex = -1;
	}
	else if (intersections->size() == 1) {
		if (intersections->at(0) > 0.0) {
			nearestObjectIndex = 0;
		}
		else {
			nearestObjectIndex = -1;
		}
	}
	else {
		nearestObjectIndex = -1;
		minimumDistance = 100000;
		for (k = 0; k < intersections->size(); k = k + 1) {
			if (minimumDistance > intersections->at(k) && intersections->at(k) > 0.0) {
				minimumDistance = intersections->at(k);
				nearestObjectIndex = k;
			}
		}
	}
	return nearestObjectIndex;
}

int Scene::findMinimumDistanceFromShadowRay(vector<double> *intersections, Ray shadowRay, int intersectionObjectIndex) {
	int k, nearestObjectIndex = 0;
	double minimumDistance, distanceFromShadowOrigin;
	Vec3 origin, direction, rayFromPOIToObject;

	origin = shadowRay.getOrigin();
	direction = shadowRay.getDirection();

	if (intersections->size() == 0) {
		nearestObjectIndex = -1;
	}
	else if (intersections->size() == 1) {
		if (intersections->at(0) > 0.0) {
			nearestObjectIndex = 0;
		}
		else {
			nearestObjectIndex = -1;
		}
	}
	else {
		nearestObjectIndex = -1;
		minimumDistance = 100000;
		for (k = 0; k < intersections->size(); k = k + 1) {
			rayFromPOIToObject = origin.vectorAddition(direction.scalarMultiplication(intersections->at(k)));
			distanceFromShadowOrigin = rayFromPOIToObject.magnitudeOfVector();
			if (minimumDistance > distanceFromShadowOrigin && intersections->at(k) > 0) {
				minimumDistance = distanceFromShadowOrigin;
				nearestObjectIndex = k;
			}
		}
	}
	if (nearestObjectIndex != intersectionObjectIndex) {
		return nearestObjectIndex;
	}
	else {
		return -1;
	}
	
}

Color Scene::phongIllumination(Vec3 lightRayForPhong, Vec3 reflectedRayForPhong, Vec3 cameraVectorForPhong, Vec3 normalAtIntersection, Color lightColor, 
	Color diffuseComponent, Color specularComponent, double shininess, double *ambientLight) {
	double red, green, blue;
	double diffuseColor, specularColor, lightDotNormal, reflectedDotCamera;

	lightDotNormal = lightRayForPhong.vectorDotProduct(normalAtIntersection);
	reflectedDotCamera = reflectedRayForPhong.vectorDotProduct(cameraVectorForPhong);

	lightDotNormal = (lightDotNormal < 0) ? 0.0 : lightDotNormal;
	reflectedDotCamera = (reflectedDotCamera < 0) ? 0.0 : reflectedDotCamera;

	red = lightColor.getRed() * (diffuseComponent.getRed() * lightDotNormal + specularComponent.getRed() * pow(reflectedDotCamera, shininess));
	green = lightColor.getGreen() * (diffuseComponent.getGreen() * lightDotNormal + specularComponent.getGreen() * pow(reflectedDotCamera, shininess));
	blue = lightColor.getBlue() * (diffuseComponent.getBlue() * lightDotNormal + specularComponent.getBlue() * pow(reflectedDotCamera, shininess));

	//red = red + KA * ambientLight[0];
	//green = green + KA * ambientLight[1];
	//blue = blue + KA * ambientLight[2];

	red = (red > 1.0) ? 1.0 : red;
	green = (green > 1.0) ? 1.0 : green;
	blue = (blue > 1.0) ? 1.0 : blue;

	return Color(red, green, blue);
	
}
Vec3 Scene::computeBaryCentricCoordinates(TriangleClass triangleIndex, Vec3 pointOfIntersection) {
	Vec3 vertexA, vertexB, vertexC, vertexP, side1, side2;
	double areaABC, areaPAB, areaPBC, alpha, beta, gamma, normalX, normalY, normalZ;

	normalX = triangleIndex.getNormalOfPlane().getX();
	normalY = triangleIndex.getNormalOfPlane().getY();
	normalZ = triangleIndex.getNormalOfPlane().getZ();

	vertexA.setX(triangleIndex.getPositionAtVertex(0).getX()); vertexA.setY(triangleIndex.getPositionAtVertex(0).getY()); vertexA.setZ(triangleIndex.getPositionAtVertex(0).getZ());
	vertexB.setX(triangleIndex.getPositionAtVertex(1).getX()); vertexB.setY(triangleIndex.getPositionAtVertex(1).getY()); vertexB.setZ(triangleIndex.getPositionAtVertex(1).getZ());
	vertexC.setX(triangleIndex.getPositionAtVertex(2).getX()); vertexC.setY(triangleIndex.getPositionAtVertex(2).getY()); vertexC.setZ(triangleIndex.getPositionAtVertex(2).getZ());
	vertexP.setX(pointOfIntersection.getX()); vertexP.setY(pointOfIntersection.getY()); vertexP.setZ(pointOfIntersection.getZ());

	areaABC = vertexA.calculateDeterminant(vertexB, vertexC);

	areaPAB = vertexP.calculateDeterminant(vertexA, vertexB);

	areaPBC = vertexP.calculateDeterminant(vertexB, vertexC); 

	alpha = areaPAB / areaABC;
	beta = areaPBC / areaABC;
	gamma = 1.0 - alpha - beta;

	return Vec3(alpha, beta, gamma);
}
void Scene::getColorAtIntersection(vector<SphereClass> *sphereIndex, vector<double> *intersections, vector<TriangleClass> *triangleIndex, 
	vector<LightClass> *lightIndex, Vec3 pointOfIntersection, Vec3 normalAtIntersection, Color *pixelColor, Ray rayFromCamera, int intersectionObjectIndex, 
	double *ambientLight) {

	int i, j, shadowExists = 0, nearestObjectIndex;
	Ray shadowRay, reflectedShadowRay;
	Vec3 lightPosition, rayToLightDirection, vectorFromPOIToObject, origin, destination, lightRayForPhong, reflectedRayForPhong, cameraVectorForPhong;
	double normalDotLight, distanceToLightSource, shadowRayPointOfIntersection, distanceFromPOIToObject, shininess;
	vector<double> intersectionOfShadowRay;
	Color lightColor, diffuseComponent, specularComponent, tempColor, dc0, dc1, dc2, sc0, sc1, sc2;
	Vec3 baryCentricCoordinates, normal;
	double alpha, beta, gamma, shininess0, shininess1, shininess2, xco, yco, zco, rampx, rampz;
	int sphereIntersection = 1, xaxis, zaxis; 
	Vec3 normal0, normal1, normal2;

	// Load the diffuse, specular components and the shininess components into the variables
	if (intersectionObjectIndex < sphereIndex->size()) {
		diffuseComponent = sphereIndex->at(intersectionObjectIndex).getSphereDiffuseColor();
		specularComponent = sphereIndex->at(intersectionObjectIndex).getSphereSpecularColor();
		shininess = sphereIndex->at(intersectionObjectIndex).getSphereShininess();

		normal = normalAtIntersection;

		sphereIntersection = 1;
	}
	else {
		// Compute the barycentric co-ordinates
		baryCentricCoordinates = this->computeBaryCentricCoordinates(triangleIndex->at(intersectionObjectIndex - sphereIndex->size()), pointOfIntersection);
		alpha = baryCentricCoordinates.getX();
		beta = baryCentricCoordinates.getY();
		gamma = baryCentricCoordinates.getZ();

		dc0 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorDiffuseAtVertex(0);
		dc1 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorDiffuseAtVertex(1);
		dc2 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorDiffuseAtVertex(2);
		
		sc0 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorSpecularAtVertex(0);
		sc1 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorSpecularAtVertex(1);
		sc2 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getColorSpecularAtVertex(2);

		diffuseComponent.setRed(alpha * dc2.getRed() + beta * dc0.getRed() + gamma * dc1.getRed());
		diffuseComponent.setGreen(alpha * dc2.getGreen() + beta * dc0.getGreen() + gamma * dc1.getGreen());
		diffuseComponent.setBlue(alpha * dc2.getBlue() + beta * dc0.getBlue() + gamma * dc1.getBlue());
		
		specularComponent.setRed(alpha * sc2.getRed() + beta * sc0.getRed() + gamma * sc1.getRed());
		specularComponent.setGreen(alpha * sc2.getGreen() + beta * sc0.getGreen() + gamma * sc1.getGreen());
		specularComponent.setBlue(alpha * sc2.getBlue() + beta * sc0.getBlue() + gamma * sc1.getBlue());

		shininess0 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getShininessAtVertex(0);
		shininess1 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getShininessAtVertex(1);
		shininess2 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getShininessAtVertex(2);

		shininess = alpha * shininess2 + beta * shininess0 + gamma * shininess1;

		normal0 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getNormalAtVertex(0);
		normal1 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getNormalAtVertex(1);
		normal2 = triangleIndex->at(intersectionObjectIndex - sphereIndex->size()).getNormalAtVertex(2);
		normal.setX(alpha * normal2.getX() + beta * normal0.getX() + gamma * normal1.getX());
		normal.setY(alpha * normal2.getY() + beta * normal0.getY() + gamma * normal1.getY());
		normal.setZ(alpha * normal2.getZ() + beta * normal0.getZ() + gamma * normal1.getZ());

		sphereIntersection = 0;
	}

	shadowRay.setOrigin(pointOfIntersection);
# ifdef SOFT_SHADOW
	for (i = 0; i < lightIndex->size(); i = i + 1) {
		for (xaxis = 0; xaxis < NUM_LIGHT_X; xaxis = xaxis + 1) {
			for (zaxis = 0; zaxis < NUM_LIGHT_Z; zaxis = zaxis + 1) {
				intersectionOfShadowRay.clear();
				xco = ((double)xaxis / (double)(20 * (NUM_LIGHT_X - 1))) - 0.025;
				yco = 0.0;
				zco = ((double)xaxis / (double)(20 * (NUM_LIGHT_Z - 1))) - 0.025;
				lightPosition = lightIndex->at(i).getLightPosition().vectorAddition(Vec3(xco, yco, zco));
				rayToLightDirection = lightPosition.vectorAddition(pointOfIntersection.invertVector());
				distanceToLightSource = rayToLightDirection.magnitudeOfVector();
				rayToLightDirection = rayToLightDirection.normalizeVector();
				shadowRay.setDirection(rayToLightDirection);

				normalDotLight = normal.vectorDotProduct(rayToLightDirection);

				if (normalDotLight > 0) {
					shadowExists = 0; // If set to zero implies shadow does not exist else if 1 then shadow exists

					// Iterate through the spheres in the scene to find the intersection point
					for (j = 0; j < sphereIndex->size(); j = j + 1) {
						shadowRayPointOfIntersection = 0;
						shadowRayPointOfIntersection = sphereIndex->at(j).intersectionWithSphere(shadowRay);
						intersectionOfShadowRay.push_back(shadowRayPointOfIntersection);
					}
					// Iterate through the triangles in the scene to find the point of intersection
					for (j = 0; j < triangleIndex->size(); j = j + 1) {
						shadowRayPointOfIntersection = 0;
						shadowRayPointOfIntersection = triangleIndex->at(j).intersectionWithTriangle(shadowRay);
						intersectionOfShadowRay.push_back(shadowRayPointOfIntersection);
					}
					nearestObjectIndex = this->findMinimumDistanceFromShadowRay(&intersectionOfShadowRay, shadowRay, intersectionObjectIndex);
					//cout << nearestObjectIndex << "\t";
					if (nearestObjectIndex == -1) {
						shadowExists = 0;
					}
					else {
						if (intersectionOfShadowRay.at(nearestObjectIndex) > DISTANCE_OFFSET) {
							origin = shadowRay.getOrigin();
							destination = shadowRay.getDirection().scalarMultiplication(intersectionOfShadowRay.at(nearestObjectIndex));
							vectorFromPOIToObject = destination.vectorAddition(origin);
							distanceFromPOIToObject = intersectionOfShadowRay.at(nearestObjectIndex);

							if ((distanceFromPOIToObject) < (distanceToLightSource - DISTANCE_OFFSET) && distanceFromPOIToObject > 0.0) {
								shadowExists = 1;
							}
							else {
								shadowExists = 0;
							}
						}
					}

					if (shadowExists == 1) {
						pixelColor->setRed(pixelColor->getRed() + SHADOW_RED);
						pixelColor->setGreen(pixelColor->getGreen() + SHADOW_GREEN);
						pixelColor->setBlue(pixelColor->getBlue() + SHADOW_BLUE);

						if (pixelColor->getRed() > 1.0) {
							pixelColor->setRed(1.0);
						}
						if (pixelColor->getGreen() > 1.0) {
							pixelColor->setGreen(1.0);
						}
						if (pixelColor->getBlue() > 1.0) {
							pixelColor->setBlue(1.0);
						}
					}
					else {
						// Compute the reflected light ray
						reflectedShadowRay = shadowRay.computeReflectedRay(normal, intersections->at(intersectionObjectIndex));

						reflectedRayForPhong = reflectedShadowRay.getDirection();
						lightRayForPhong = shadowRay.getDirection();
						cameraVectorForPhong = rayFromCamera.getDirection().invertVector();
						//lightColor = lightIndex->at(i).getLightColor();
						rampx = ((2.0 * (double)xaxis / (double)NUM_LIGHT_X));
						rampz = ((2.0 * (double)zaxis / (double)NUM_LIGHT_Z));
						lightColor.setRed(lightIndex->at(i).getLightColor().getRed() * rampx * rampz / (double)(NUM_LIGHT_X * NUM_LIGHT_Z));
						lightColor.setGreen(lightIndex->at(i).getLightColor().getGreen() * rampx * rampz / (double)(NUM_LIGHT_X * NUM_LIGHT_Z));
						lightColor.setBlue(lightIndex->at(i).getLightColor().getBlue() * rampx * rampz / (double)(NUM_LIGHT_X * NUM_LIGHT_Z));
						// Compute the light using the phong illumnation model

						tempColor = phongIllumination(lightRayForPhong, reflectedRayForPhong, cameraVectorForPhong, normal, lightColor, diffuseComponent,
							specularComponent, shininess, ambientLight);
						pixelColor->setRed(pixelColor->getRed() + tempColor.getRed());
						pixelColor->setGreen(pixelColor->getGreen() + tempColor.getGreen());
						pixelColor->setBlue(pixelColor->getBlue() + tempColor.getBlue());
						if (pixelColor->getRed() > 1.0) {
							pixelColor->setRed(1.0);
						}
						if (pixelColor->getGreen() > 1.0) {
							pixelColor->setGreen(1.0);
						}
						if (pixelColor->getBlue() > 1.0) {
							pixelColor->setBlue(1.0);
						}
					}
				}
				else {
					pixelColor->setRed(pixelColor->getRed() + SHADOW_RED);
					pixelColor->setGreen(pixelColor->getGreen() + SHADOW_GREEN);
					pixelColor->setBlue(pixelColor->getBlue() + SHADOW_BLUE);

					if (pixelColor->getRed() > 1.0) {
						pixelColor->setRed(1.0);
					}
					if (pixelColor->getGreen() > 1.0) {
						pixelColor->setGreen(1.0);
					}
					if (pixelColor->getBlue() > 1.0) {
						pixelColor->setBlue(1.0);
					}
				}
			}
			}
			pixelColor->setRed(pixelColor->getRed() + KA * ambientLight[0]);
			pixelColor->setGreen(pixelColor->getGreen() + KA * ambientLight[1]);
			pixelColor->setBlue(pixelColor->getBlue() + KA * ambientLight[2]);
		}

# else
	for (i = 0; i < lightIndex->size(); i = i + 1) {
		intersectionOfShadowRay.clear();
		lightPosition = lightIndex->at(i).getLightPosition();
		rayToLightDirection = lightPosition.vectorAddition(pointOfIntersection.invertVector());
		distanceToLightSource = rayToLightDirection.magnitudeOfVector();
		rayToLightDirection = rayToLightDirection.normalizeVector();
		shadowRay.setDirection(rayToLightDirection);

		normalDotLight = normal.vectorDotProduct(rayToLightDirection);

		if (normalDotLight > 0) {
			shadowExists = 0; // If set to zero implies shadow does not exist else if 1 then shadow exists

			// Iterate through the spheres in the scene to find the intersection point
			for (j = 0; j < sphereIndex->size(); j = j + 1) {
				shadowRayPointOfIntersection = 0;
				shadowRayPointOfIntersection = sphereIndex->at(j).intersectionWithSphere(shadowRay);
				intersectionOfShadowRay.push_back(shadowRayPointOfIntersection);
			}
			// Iterate through the triangles in the scene to find the point of intersection
			for (j = 0; j < triangleIndex->size(); j = j + 1) {
				shadowRayPointOfIntersection = 0;
				shadowRayPointOfIntersection = triangleIndex->at(j).intersectionWithTriangle(shadowRay);
				intersectionOfShadowRay.push_back(shadowRayPointOfIntersection);
			}
			nearestObjectIndex = this->findMinimumDistanceFromShadowRay(&intersectionOfShadowRay, shadowRay, intersectionObjectIndex);
			//cout << nearestObjectIndex << "\t";
			if (nearestObjectIndex == -1) {
				shadowExists = 0;
			}
			else {
				if (intersectionOfShadowRay.at(nearestObjectIndex) > DISTANCE_OFFSET) {
					origin = shadowRay.getOrigin();
					destination = shadowRay.getDirection().scalarMultiplication(intersectionOfShadowRay.at(nearestObjectIndex));
					vectorFromPOIToObject = destination.vectorAddition(origin);
					distanceFromPOIToObject = intersectionOfShadowRay.at(nearestObjectIndex);

					if ((distanceFromPOIToObject) < (distanceToLightSource - DISTANCE_OFFSET) && distanceFromPOIToObject > 0.0) {
						shadowExists = 1;
					}
					else {
						shadowExists = 0;
					}
				} 
			}
			
			if (shadowExists == 1) {
				pixelColor->setRed(pixelColor->getRed() + SHADOW_RED);
				pixelColor->setGreen(pixelColor->getGreen() + SHADOW_GREEN);
				pixelColor->setBlue(pixelColor->getBlue() + SHADOW_BLUE);

				if (pixelColor->getRed() > 1.0) {
					pixelColor->setRed(1.0);
				}
				if (pixelColor->getGreen() > 1.0) {
					pixelColor->setGreen(1.0);
				}
				if (pixelColor->getBlue() > 1.0) {
					pixelColor->setBlue(1.0);
				}
			}
			else {
				// Compute the reflected light ray
				reflectedShadowRay = shadowRay.computeReflectedRay(normal, intersections->at(intersectionObjectIndex));

				reflectedRayForPhong = reflectedShadowRay.getDirection();
				lightRayForPhong = shadowRay.getDirection();
				cameraVectorForPhong = rayFromCamera.getDirection().invertVector();
				lightColor = lightIndex->at(i).getLightColor();
				// Compute the light using the phong illumnation model

				tempColor = phongIllumination(lightRayForPhong, reflectedRayForPhong, cameraVectorForPhong, normal, lightColor, diffuseComponent, 
					specularComponent, shininess, ambientLight);
				pixelColor->setRed(pixelColor->getRed() + tempColor.getRed());
				pixelColor->setGreen(pixelColor->getGreen() + tempColor.getGreen());
				pixelColor->setBlue(pixelColor->getBlue() + tempColor.getBlue());
				if (pixelColor->getRed() > 1.0) {
					pixelColor->setRed(1.0);
				}
				if (pixelColor->getGreen() > 1.0) {
					pixelColor->setGreen(1.0);
				}
				if (pixelColor->getBlue() > 1.0) {
					pixelColor->setBlue(1.0);
				}
			}
		}
		else {
			pixelColor->setRed(pixelColor->getRed() + SHADOW_RED);
			pixelColor->setGreen(pixelColor->getGreen() + SHADOW_GREEN);
			pixelColor->setBlue(pixelColor->getBlue() + SHADOW_BLUE);

			if (pixelColor->getRed() > 1.0) {
				pixelColor->setRed(1.0);
			}
			if (pixelColor->getGreen() > 1.0) {
				pixelColor->setGreen(1.0);
			}
			if (pixelColor->getBlue() > 1.0) {
				pixelColor->setBlue(1.0);
			}
		}
		pixelColor->setRed(pixelColor->getRed() + KA * ambientLight[0]);
		pixelColor->setGreen(pixelColor->getGreen() + KA * ambientLight[1]);
		pixelColor->setBlue(pixelColor->getBlue() + KA * ambientLight[2]);
	}
# endif
}

// Trace function traces the ray from the camera position to the point in the 3D world, to obtain the color of the pixel that the
// ray traces from the camera. The color of the object is loaded into the pixel.
void Scene::trace(vector<SphereClass> *sphereIndex, vector<double> *intersections, vector<TriangleClass> *triangleIndex, vector<LightClass> *lightIndex, 
	Ray rayFromCamera, Color *pixelColor, double *ambientLight) {
	double intersectionPoint, scalar;
	int k, nearestObjectIndex = 0;
	Vec3 pointOfIntersection, rayOrigin, rayDirection, scalarTimesRayDirection, normalAtIntersection;
	Ray reflectedRay, transmittedRay;
	if (numOfRebounds > maxRebounds) {
		pixelColor->setRed(1.0);
		pixelColor->setGreen(1.0);
		pixelColor->setBlue(1.0);
	}
	else {
		// Code to obtain the intersection with spheres
		for (k = 0; k < sphereIndex->size(); k = k + 1) {
			intersectionPoint = sphereIndex->at(k).intersectionWithSphere(rayFromCamera);
			intersections->push_back(intersectionPoint);
		}
		// Code to obtain the intersection with triangles
		for (k = 0; k < triangleIndex->size(); k = k + 1) {
			intersectionPoint = triangleIndex->at(k).intersectionWithTriangle(rayFromCamera);
			intersections->push_back(intersectionPoint);
		}

		// Find the minimum of the intersection points and that minimum value index is used to determine the 
		// intersection of the line with the object present in the screen
		nearestObjectIndex = this->findMinimumDistance(intersections);


		if (nearestObjectIndex == -1) {
			pixelColor->setRed(1.0);
			pixelColor->setGreen(1.0);
			pixelColor->setBlue(1.0);
		}
		else {
			numOfRebounds = numOfRebounds + 1;
			if (intersections->at(nearestObjectIndex) > DISTANCE_OFFSET) {
				// Find the point of intersection
				rayOrigin = rayFromCamera.getOrigin();
				rayDirection = rayFromCamera.getDirection();
				scalar = intersections->at(nearestObjectIndex);
				scalarTimesRayDirection = rayDirection.scalarMultiplication(scalar);

				pointOfIntersection.setX(rayOrigin.vectorAddition(scalarTimesRayDirection).getX());
				pointOfIntersection.setY(rayOrigin.vectorAddition(scalarTimesRayDirection).getY());
				pointOfIntersection.setZ(rayOrigin.vectorAddition(scalarTimesRayDirection).getZ());

				// Find the normal at the point of intersection
				if (nearestObjectIndex < sphereIndex->size()) {
					normalAtIntersection = sphereIndex->at(nearestObjectIndex).normalAtIntersection(pointOfIntersection);
				}
				else {
					normalAtIntersection = triangleIndex->at(nearestObjectIndex - sphereIndex->size()).getNormalOfPlane();
				}

				// Obtain the color at the point of intersection
				getColorAtIntersection(sphereIndex, intersections, triangleIndex, lightIndex, pointOfIntersection,
					normalAtIntersection, pixelColor, rayFromCamera, nearestObjectIndex, ambientLight);
				// Compute the reflected ray wrt the ray shot from the camera
				reflectedRay = rayFromCamera.computeReflectedRay(normalAtIntersection, scalar);
				Color reflectedColor(BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE);
				vector<double> reflectedIntersections;
				// Recursive ray tracing
				trace(sphereIndex, &reflectedIntersections, triangleIndex, lightIndex, reflectedRay, &reflectedColor, ambientLight);

				pixelColor->setRed(((1 - KS) * pixelColor->getRed()) + (KS * reflectedColor.getRed()));
				pixelColor->setGreen(((1 - KS) * pixelColor->getGreen()) + (KS * reflectedColor.getGreen()));
				pixelColor->setBlue(((1 - KS) * pixelColor->getBlue()) + (KS * reflectedColor.getBlue()));

				if (pixelColor->getRed() > 1.0) {
					pixelColor->setRed(1.0);
				}
				if (pixelColor->getGreen() > 1.0) {
					pixelColor->setGreen(1.0);
				}
				if (pixelColor->getBlue() > 1.0) {
					pixelColor->setBlue(1.0);
				}
			}
		}
	}
}