#ifndef RAYTRACER_VERTEX_H
#define RAYTRACER_VERTEX_H

#include "Vector3.h"

class Vertex {
public:
	Color color;
	Vector3 position;
	bool isTransitionUpdated = false;
	bool isScalingUpdated = false;
	bool isRotationUpdated = false;

	int x;
	int y;
	float z;
};

#endif //RAYTRACER_VERTEX_H
