#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <vector>
#include "Vector3.h"
#include "Image.h"

class Camera {
public:
	struct {
		Vector3 Forward, Up, Left;
	} _space;

	struct {
		float Left, Right, Bottom, Top;
		float Near;
		float Far;

		int Width;
		int Height;

	} _imagePlane;

	Vector3 _position;
	Vector3 _gaze;

	std::string _outputFile;

	float** mvp;
	float** mper;
	float** mcam;

public:

	/*
	 * BEWARE! You should render each camera by using this function, we will grade it according to regulations!
	 */
	Image Render() const;

	friend std::istream& operator>>(std::istream& stream, Camera& camera);

	const std::string& OutputFile() const {
		return _outputFile;
	}
};

#endif //RAYTRACER_CAMERA_H
