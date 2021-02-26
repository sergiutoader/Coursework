#include "Collisions.h"
#include <cmath>

// functie de detectare coliziune Punct - Cerc
bool PointToCircleCollision(float x, float y, float xc, float yc, float radius) {

	float dx = x - xc;
	float dy = y - yc;

	float dist = sqrt(dx * dx + dy * dy);
	if (dist <= radius) {
		return true;
	}
	return false;
}

// functie de detectare coliziune Punct - Elipsa
bool PointToEllipseCollision(float x, float y, float xc, float yc, float radius, float a, float b) {

	float dx = x - xc;
	float dy = y - yc;

	float dist = sqrt(dx * dx / (a * a) + dy * dy / (b * b));
	if (dist <= radius) {
		return true;
	}
	return false;
}

// functie de detectare coliziune Cerc - Cerc
bool CircleToCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2) {

	float dx = x1 - x2;
	float dy = y1 - y2;

	float dist = sqrt(dx * dx + dy * dy);
	if (dist <= r1 + r2) {
		return true;
	}
	return false;
}