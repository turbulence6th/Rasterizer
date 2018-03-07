#ifndef RAYTRACER_MODEL_H
#define RAYTRACER_MODEL_H

#include "Vector3.h"
#include "Triangle.h"
#include <vector>

class Model {
public:
	int modelType;
	std::vector<Triangle> _triangles;
	std::vector<std::pair<char, int> > _transformation;
};

#endif
